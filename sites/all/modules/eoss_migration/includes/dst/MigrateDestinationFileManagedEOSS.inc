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

    // Set up directory names
    $this->srcFileURI = MigrateWebformFileDirectories::getSrcFolder() . $source_row->filename;

    // @TODO - Get single node directory inserted right V here.

    var_dump(__METHOD__);
    var_dump($entity);

    $file_info = MigrateWebformFileDirectories::getFileInfoFromCustomMap($entity->fid);
    $this->dstFileURI = MigrateWebformFileDirectories::getDstFolder() . $file_info->uuid
      . '/' . $entity->filename;

    // DEBUG
    $migration->saveMessage('srcFileURI:: ' . $this->srcFileURI, MigrationBase::MESSAGE_INFORMATIONAL);
    $migration->saveMessage('dstFileURI:: ' . $this->dstFileURI, MigrationBase::MESSAGE_INFORMATIONAL);

    // Move the files
    if (!rename($this->srcFileURI, $this->dstFileURI)) {
      $migration->saveMessage('[FAIL-I] - Filename ' . $source_row->filename . ' could not be moved.',
        MigrationBase::MESSAGE_ERROR);
    }
    else {
      touch($this->srcFileURI, $source_row->timestamp); // reset timestamp
      // DEBUG
      $migration->saveMessage('[SUCCESS-I] Filename ' . $source_row->filename . ' moved to '
        . MigrateWebformFileDirectories::getDstFolder(), MigrationBase::MESSAGE_INFORMATIONAL);
    }

  }


  /**
   * Rolls back URIs from custom, new directory information to original directories,
   * based on mapping(?)
   *
   * See parent::prepareRollback.
   * @param array $ids
   */
  public function prepareRollback(array $ids) {
    parent::prepareRollback($ids);

    // Move files back before rollBack (the ID will be lost before the file move can happen)
    $migration = Migration::currentMigration();

    var_dump(__METHOD__);
    var_dump("-----------------");
    var_dump($ids);
    var_dump("-----------------");

    // Manually repull row here before rollback of files
    if (isset($ids['fid'])) {
      $file_record = $this->getFileTimestamp($ids['fid']);

      // @TODO - Get getFileInfoFromCustomMap working
      $file_info = MigrateWebformFileDirectories::getFileInfoFromCustomMap($ids['fid']);
      var_dump(__METHOD__);
      var_dump($file_info);

      foreach ($file_record as $record) {
//        $this->srcFileURI = MigrateWebformFileDirectories::getSrcFolder() . $record->filename;
//        $this->dstFileURI = MigrateWebformFileDirectories::getDstFolder() . '/' . $file_info->uuid . '/' . $record->filename;

        // DEBUG
        $migration->saveMessage('srcFileURI:: ' . $this->srcFileURI, MigrationBase::MESSAGE_INFORMATIONAL);
        $migration->saveMessage('dstFileURI:: ' . $this->dstFileURI, MigrationBase::MESSAGE_INFORMATIONAL);

        // Move files back...
        if (rename($this->dstFileURI, $this->srcFileURI)) {
          // Then reset the modified date
          touch($this->srcFileURI, $record->timestamp); // reset timestamp
          $migration->displayMessage('[SUCCESS-R] File ' . $this->dstFileURI
            . ' moved back to ' . MigrateWebformFileDirectories::getSrcFolder(), MigrationBase::MESSAGE_INFORMATIONAL);
        }
      }
    }
  }

  // @TODO - Implement postRollBack to delete empty webform files directories after completeRollbacks are all called

  /**
   * Gets timestamp of file before their migration
   *
   * @param $ids
   * @return (obj) original filename + timestamp of file upload from DB
   */
  private function getFileTimestamp(string $ids) {
    $file_record = Database::getConnection('default', 'default')
      ->select('file_managed', 'fm')
      ->fields('fm', array('filename', 'timestamp'))
      ->condition('fm.fid', $ids['fid'])
      ->execute()->fetchObject();
    return $file_record;
  }
}