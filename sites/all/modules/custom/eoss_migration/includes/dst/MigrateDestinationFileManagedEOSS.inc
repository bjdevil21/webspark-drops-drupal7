<?php

/**
 * Migrate API destination handler
 *
 * DB tables
 * - file_managed
 * - file_usage (stubs)
 *
 * Handling of the actual files related to the file_managed + file_usage DB tables.
 *
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
   * - New download folders are generated on the fly for new directories, if needed.
   *   (public folders generated in preImport call).
   *
   * @param $entity
   * @param \stdClass $source_row
   */
  public function complete($entity, stdClass $source_row) {
    parent::complete($entity, $source_row);

    $migration = Migration::currentMigration();

    // Get SRC directory name
    $src_uri_base = MigrateWebformFilesLibrary::isUriPrivate($source_row->uri, FALSE);

    // Get REAL filename (in case of duplicate renaming by Drupal upon upload)
    $true_filename = $this->getRealFileName($source_row->uri, $source_row->filename);

    $this->srcFileURI = MigrateWebformFilesLibrary::getAbsFileFolder('src') . $src_uri_base . $true_filename;

    // Set up DST directory name
    $dst_uri_base = (MigrateWebformFileManagedEOSS::MIGRATE_FORCE_PRIVATE_FILES == 1)
      ? "private/webform/"
      : MigrateWebformFilesLibrary::isUriPrivate($entity->uri, FALSE);

    /// Convoluted path to new File URI
    $nid_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($source_row->fid, 'nid');
    $directory = new MigrateWebformFilesLibrary($nid_result->nid, 'nid');
    $dst_dir_base = MigrateWebformFilesLibrary::getAbsFileFolder('dst') . $dst_uri_base . $directory->singleDir;
    $this->dstFileURI = $dst_dir_base . '/' . $true_filename;

    if (file_exists($this->srcFileURI)) {
      // First check if directory exists. If not, create it before attempting file move
      if (!file_exists($dst_dir_base) && !is_dir($dst_dir_base)) {
        if (drupal_mkdir($dst_dir_base)) {
          // Create record of folder creation (or existence).
          // Zeroes will denote a newly created record here.
          $result = new stdClass();
          $result->nid = 0;
          $result->cid = 0;
          MigrateWebformFileManagedEOSS::createFolderRecord($result, $directory->singleDir, $dst_dir_base);

          MigrationBase::displayMessage('[SUCCESS-I] - New directory ' . $dst_dir_base . ' created for '
            . $true_filename, 'status');
        }
      } else {
        //MigrationBase::displayMessage('[SKIP-I] - Directory ' . $dst_dir_base . ' already exists or access is denied.', 'status');
      }

      // Now move the actual files
      if (rename($this->srcFileURI, $this->dstFileURI)) {
        //touch($this->srcFileURI, $source_row->timestamp); // reset timestamp
        $migration->saveMessage('[SUCCESS-I] Moved filename ' . $true_filename . ' to '
            . $this->dstFileURI . $directory->singleDir, MigrationBase::MESSAGE_INFORMATIONAL);
      }
      else {
        $migration->saveMessage('[FAIL-I] - Could not move filename ' . $true_filename . '.', MigrationBase::MESSAGE_ERROR);
      }
    }
    else {
      $migration->saveMessage('[WARNING-I] Filename ' . $true_filename . ' does not exist at '
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
      $dst_uri_base = (MigrateWebformFileManagedEOSS::MIGRATE_FORCE_PRIVATE_FILES == 1)
        ? 'private/webform/'
        : MigrateWebformFilesLibrary::isUriPrivate($dst_fid_result->uri, FALSE);

      $true_filename = $this->getRealFileName($src_fid_result->uri, $src_fid_result->filename);

      $this->srcFileURI = MigrateWebformFilesLibrary::getAbsFileFolder('src')
        . $src_uri_base . $true_filename;
      $this->dstFileURI = MigrateWebformFilesLibrary::getAbsFileFolder('dst')
        . $dst_uri_base . $directory->singleDir . '/' . $true_filename;

      // Move files back to src directories
      if (file_exists($this->dstFileURI)) {
        if (rename($this->dstFileURI, $this->srcFileURI)) {
          // Then reset the modified date
          touch($this->srcFileURI, $dst_fid_result->timestamp); // reset timestamp
//          MigrationBase::displayMessage('[SUCCESS-R] Moved file ' . $dst_fid_result->filename . ' back to ' . $this->srcFileURI, 'status');
        }
        else {
          MigrationBase::displayMessage('[FAIL-R] Could not move file ' . $dst_fid_result->filename
            . ' back to ' . $this->srcFileURI, 'warning');
        }
      }
      else {
        MigrationBase::displayMessage('[FAIL-R] File ' . $dst_fid_result->filename
          . ' does not exist at ' . $this->dstFileURI, 'error');
      }
      // File move pass or fail, delete referenced file_usage record (stub)
      db_delete('file_usage')
        ->condition('fid', $fid)
        ->execute();
    }
  }

  /**
   * Delete empty webform file directories after rollback is finished.
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
          Migration::displayMessage('[WARNING-R] - Could not delete ' . $dir->directory_full . ' directory.', 'warning');
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

  /**
   * Get actual file name from URI, just in case there was a duplicate file and an
   * _\d was attached to the file name.
   * @param string $uri - URI to parse for filename
   * @param string $filename - Assumed filename from file_managed.filename record
   * @return - Actual file name to be used for finding and moving files
   */
  private function getRealFileName($uri, $filename) {
    $real = preg_replace('|^.+\/([^\/]+?)$|', "$1", $uri);
    return ($real !== $filename) ? $real : $filename;
  }
}
