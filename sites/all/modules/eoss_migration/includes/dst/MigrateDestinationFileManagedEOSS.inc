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

  public function complete($entity, stdClass $source_row) {
    parent::complete($entity, $source_row);

    $migration = Migration::currentMigration();

    // #4 - Move files from eoss-migrate (local) to eoss-forms (local)

    // Set up SRC directory name
    $this->srcFileURI = MigrateWebformFilesLibrary::isUriPrivate($source_row->uri, 'src_uri', TRUE) . $source_row->filename;

    // Set up DST directory name
    $nid_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($source_row->fid, 'nid');
    $directory = new MigrateWebformFilesLibrary($nid_result->nid, 'nid');
    $dst_dir_base = MigrateWebformFilesLibrary::isUriPrivate($entity->uri, 'dst', TRUE) . $directory->singleDir;
    $this->dstFileURI = $dst_dir_base . '/' . $entity->filename;

    if (file_exists($this->srcFileURI)) {
      // First check if directory exists. If not, create it before attempting file move
      if (!file_exists($dst_dir_base) && !is_dir($dst_dir_base) && (file_prepare_directory($dst_dir_base))) {
        if (drupal_mkdir($dst_dir_base)) {
          // Create record of folder creation (or existence)
          db_merge('migrate_map_custom_files_dirs')
            ->key(array(
              'directory' => $directory->singleDir,
            ))
            ->fields(array(
              'directory_full' => $dst_dir_base, // nid, cid = 0 (will note a custom entry)
            ))
            ->execute();
          MigrationBase::displayMessage('Directory ' . $dst_dir_base . ' created.', MigrationBase::MESSAGE_INFORMATIONAL);
        }
      }
      // Now move the actual files
      if (rename($this->srcFileURI, $this->dstFileURI)) {
        touch($this->srcFileURI, $source_row->timestamp); // reset timestamp
        $migration->saveMessage('[SUCCESS-I] Filename ' . $source_row->filename . ' moved to '
            . $this->dstFileURI . $directory->singleDir, MigrationBase::MESSAGE_INFORMATIONAL);
      }
      else {
        $migration->saveMessage('[FAIL-I] - Filename ' . $source_row->filename . ' could not be moved.', MigrationBase::MESSAGE_ERROR);
      }
    }
  }

  /**
   * Rolls back fm.uri values from custom, new directory information to original directories
   *
   * See parent::prepareRollback.
   * @param array $ids
   */
  public function prepareRollback(array $ids) {
    parent::prepareRollback($ids);

    // Move files back before rollBack (the ID will be lost before the file move can happen)
    $migration = Migration::currentMigration();

    // Manually repull row here before rollback of files
    $fid = (int) $ids['fid'];

    if (is_int($fid)) {

      // Roll back URI values
      $dst_fid_record = $this->getDstFileRecord($fid); // Will only return one object.

      $src_fid_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($fid, 'src_fid'); // All old fm data
      $uuid_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($src_fid_result->fid, 'n_uuid'); // n.uuid

      $this->srcFileURI = MigrateWebformFilesLibrary::isUriPrivate($src_fid_result->uri, 'src', TRUE) . '/' . $src_fid_result->filename;
      $this->dstFileURI = MigrateWebformFilesLibrary::isUriPrivate($dst_fid_record->uri, 'dst', TRUE) . '/' . $uuid_result->uuid . '/' . $dst_fid_record->filename;

      // Move files back to src directories
      if (file_exists($this->dstFileURI)) {
        if (rename($this->dstFileURI, $this->srcFileURI)) {
          // Then reset the modified date
          touch($this->srcFileURI, $dst_fid_record->timestamp); // reset timestamp
          $migration->displayMessage('[SUCCESS-R] File ' . $dst_fid_record->filename
            . ' moved back to ' . $this->srcFileURI, MigrationBase::MESSAGE_INFORMATIONAL);
        }
        else {
          $migration->displayMessage('[FAIL-R] File ' . $dst_fid_record->filename
            . ' could not be moved back to ' . $this->srcFileURI, MigrationBase::MESSAGE_INFORMATIONAL);
        }
        // Delete any attached file_usage stubs created.
        db_delete('file_usage')
          ->condition('fid', $fid)
          ->execute();
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
      $result = file_scan_directory($dir->directory_full, '/.*/');
      if (count($result) == 0) {
        drupal_rmdir($dir);
      }
      // Delete remaining mapping entry
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