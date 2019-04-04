<?php

/**
 * file_managed - Adds file handling of the actual files related to the file_managed records..
 */
class MigrateDestinationFileManagedEOSS extends MigrateDestinationTable {
  /**
   * Filename to be moved.
   * @var string
   */
  private $srcFileURI;

  private $dstFileURI;

  protected $fileInfo;

  /**
   * MigrateDestinationFileManagedEOSS constructor.
   * @param $table_name
   */
  public function __construct($table_name) {
    parent::__construct($table_name);
  }

  /**
   * - Creates file_usage entries
   * - Moves file_managed referenced files from src -> dst, putting each file in
   *   a different folder.
   * - New download folders are generated on the fly for new private directories, if needed.
   *   (public folders generated in preImport call).
   *
   * @param $entity
   * @param \stdClass $source_row
   */
  public function complete($entity, stdClass $source_row) {
    parent::complete($entity, $source_row);

    $migration = Migration::currentMigration();

    // Set up SRC directory name
    $src_uri_base = MigrateWebformFilesLibrary::isUriPrivate($source_row->uri, FALSE);
    $this->srcFileURI = MigrateWebformFilesLibrary::getAbsFileFolder('src')
      . $src_uri_base . $source_row->filename;

    // Set up DST directory name
    $dst_uri_base = MigrateWebformFilesLibrary::isUriPrivate($entity->uri, FALSE);
    ////
    $nid_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($source_row->fid, 'nid');
    $directory = new MigrateWebformFilesLibrary($nid_result->nid, 'nid');
    $dst_dir_base = MigrateWebformFilesLibrary::getAbsFileFolder('dst') . $dst_uri_base . $directory->singleDir;
    $this->dstFileURI = $dst_dir_base . '/' . $entity->filename;

    if (file_exists($this->srcFileURI)) {
      // First check if directory exists. If not, create it before attempting file move
      if (!file_exists($dst_dir_base) && !is_dir($dst_dir_base)) {
        if (drupal_mkdir($dst_dir_base)) {
          // Create record of folder creation (or existence)
          db_merge('migrate_map_custom_files_dirs')
            ->key(array(
              'directory_full' => $dst_dir_base,
            ))
            ->fields(array(
              'directory' => $directory->singleDir,
              'directory_full' => $dst_dir_base,
              'nid' => 0, // (default 0 will denote a custom entry)
              'cid' => 0, // (default 0 will denote a custom entry)
            ))
            ->execute();
          MigrationBase::displayMessage('[SUCCESS-I] - New directory ' . $dst_dir_base . ' created for '
            . $entity->filename, 'status');
        }
      } else {
        //MigrationBase::displayMessage('[SKIP-I] - Directory ' . $dst_dir_base . ' already exists or access is denied.', 'status');
      }

      // Now move the actual files
      if (rename($this->srcFileURI, $this->dstFileURI)) {
        //touch($this->srcFileURI, $source_row->timestamp); // reset timestamp
        $migration->saveMessage('[SUCCESS-I] Moved filename ' . $source_row->filename . ' to '
            . $this->dstFileURI . $directory->singleDir, MigrationBase::MESSAGE_INFORMATIONAL);
      }
      else {
        $migration->saveMessage('[FAIL-I] - Could not move filename ' . $source_row->filename . '.', MigrationBase::MESSAGE_ERROR);
      }
    }
    else {
      $migration->saveMessage('[WARNING-I] Filename ' . $source_row->filename . ' does not exist at '
        . $this->srcFileURI, MigrationBase::MESSAGE_WARNING);
    }
  }

  /**
   * - Rolls back fm.uri values from new directory information to original directory entries on src.
   * - Moves files back to original src directory locations (and deletes the custom directories on dst)
   * - Deletes file_usage entries generated in prepareRow (as stubs)
   *
   * See parent::prepareRollback.
   * @param array $ids
   */
  public function prepareRollback(array $ids) {
    parent::prepareRollback($ids);

    $migration = Migration::currentMigration();
    $fid = (int) $ids['fid'];

    if (is_int($fid)) {
      // Repull fm row here before rollback of files
      $dst_fid_result = $this->getDstFileRecord($fid); // Will return one file_managed result object.
      $dst_fid_result = $dst_fid_result[0]; // Take out of array and return only object
      $src_fid_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($fid, 'src_fid'); // All old fm data
      $nid_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($src_fid_result->fid, 'nid'); // new n.nid
      $directory = new MigrateWebformFilesLibrary($nid_result->nid);

      // Use same URI bases for now - won't stay that way if migration includes public -> private transition...
      $src_uri_base = MigrateWebformFilesLibrary::isUriPrivate($src_fid_result->uri, FALSE);
      $dst_uri_base = MigrateWebformFilesLibrary::isUriPrivate($dst_fid_result->uri, FALSE);

      $this->srcFileURI = MigrateWebformFilesLibrary::getAbsFileFolder('src')
        . $src_uri_base . $src_fid_result->filename;
      $this->dstFileURI = MigrateWebformFilesLibrary::getAbsFileFolder('dst')
        . $dst_uri_base . $directory->singleDir . '/' . $dst_fid_result->filename;

      // Move files back to src directories
      if (file_exists($this->dstFileURI)) {
        if (rename($this->dstFileURI, $this->srcFileURI)) {
          // Then reset the modified date
          touch($this->srcFileURI, $dst_fid_result->timestamp); // reset timestamp
          MigrationBase::displayMessage('[SUCCESS-R] Moved file ' . $dst_fid_result->filename
            . ' back to ' . $this->srcFileURI, 'status');
        }
        else {
          MigrationBase::displayMessage('[FAIL-R] Could not move file ' . $dst_fid_result->filename
            . ' back to ' . $this->srcFileURI, 'warning');
        }
        // Delete any attached file_usage stubs created.
        db_delete('file_usage')
          ->condition('fid', $fid)
          ->execute();
      }
      else {
        MigrationBase::displayMessage('[FAIL-R] File ' . $dst_fid_result->filename
          . ' does not exist at ' . $this->dstFileURI, 'error');
      }
    }
  }

  /**
   * @TODO Implement postRollBack to delete empty webform files directories after completeRollbacks are all called
   */
  public function postRollback() {
    // Call up all the directories
    $dirs = Database::getConnection()
      ->select('migrate_map_custom_files_dirs', 'mf')
      ->fields('mf')
      ->execute()
      ->fetchAll();
    foreach ($dirs as $dir) {
      // Check if empty, and delete if yes
      $result = file_scan_directory($dir->directory_full, '/^.+$/');
      if (count($result) == 0) {
        if (drupal_rmdir($dir->directory_full)) {
          Migration::displayMessage('[SUCCESS-R] - Deleted directory ' . $dir->directory_full . '.', 'status');
        } else {
          Migration::displayMessage('[FAIL-R] - Could not delete ' . $dir->directory_full . ' directory.', 'warning');
        }
      }
      else {
        Migration::displayMessage('[FAIL-R] Non-empty directory ' . $dir->directory_full . '. Skipping delete.', 'warning');
      }
      // Delete all files_dirs mapping entries
      db_delete('migrate_map_custom_files_dirs')
        ->condition('id', $dir->id)
        ->execute();
    }
  }

  /**
   * Get dst file_managed record
   * @param int $fid - Destination file ID
   * @return bool
   */
  private function getDstFileRecord($fid) {
    $dst_file_record = Database::getConnection('default', 'default')
      ->select('file_managed', 'fm')
      ->fields('fm')
      ->condition('fm.fid', $fid)
      ->execute()->fetchAll();
    if (count($dst_file_record) <> 1) {
      Migration::displayMessage(__METHOD__ . ' - Number of records for source file ID ' . $fid . ' is not PRECISELY 1.');
      return FALSE;
    }
    else {
      return $dst_file_record;
    }
  }
}