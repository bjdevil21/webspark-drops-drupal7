<?php

/**
 * Created by PhpStorm.
 * User: bryan
 * Date: 3/19/19
 * Time: 2:49 PM
 */
class MigrateWebformFileManagedEOSS extends DrupalMigration {

  public function __construct($arguments) {
    parent::__construct($arguments);

    $table_name = 'file_managed';

    $this->source = new MigrateSourceSQL(
      $this->query(),
      $this->getFieldCols('all'),
      NULL,
      array(
        'map_joinable' => FALSE,
        'skip_count' => FALSE
      )
    );

    $this->destination = new MigrateDestinationTable($table_name);

    $this->addSimpleMappings($this->getFieldCols('simple'));

    // file ID mapping
    // @TODO - add in createStub to make file_usage stubs, or do file_usage first
    $this->addFieldMapping('fid', 'fid');

    // Default uid to 0 if we're not mapping it.
    $this->removeFieldMapping('uid');
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('DrupalUser7MigrationEOSS')
      ->defaultValue(0); // Anonymous user

    $mappable_key = array(
      'fid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'File ID',
        'alias' => 'fm',
      ),
    );

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $mappable_key,
      MigrateDestinationTable::getKeySchema($table_name)
    );
  }

  // @TODO - Select query to grab all files
  public function query() {
    $fm_query = Database::getConnection('default', $this->sourceConnection)
      ->select('file_managed', 'fm');
    $fm_query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
    $fm_query->fields('fm', $this->getFieldCols('all'))
      ->condition('fm.fid', '', '>') // debugging condition
      ->condition('fu.module', 'webform', 'LIKE') // All attached file entries
      ->orderBy('fm.fid', 'ASC');
    return $fm_query;
  }

  /**
   * @param $row
   * @return bool
   */
  public function prepareRow($row) {
    // Skip record validation
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

//        MigrationBase::displayMessage('[SKIP] - Webform does not exist for submission ID ' . $row->sid, 'status');
  }

  public function prepare($entity, $row) {
    // Remove the old File ID from $entity (not $row),
    // so the insert can create the new record and map it properly.
    unset($entity->fid);
  }

  /**
   * @param string $cols
   * @return array of webform_submission DB cols
   */
  public function getFieldCols($cols = 'all') {
    $columns = new MigrateFieldsEOSS();
    $new = $columns->whichFileManagedCols($cols);
    return $new;
  }

  /**
   * Create stub for file_managed table
   *
   * @param $migration - Object that has migration data
   * @param $ids - new ID to inject
   */
  public function createStub($migration, array $ids) {

    $migration = Migration::currentMigration();

    MigrationBase::displayMessage(__METHOD__ . ' -FM == ids- ' . var_dump($ids), 'status');
    MigrationBase::displayMessage('FFFFFFFFFFFFFFFFFFFMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM----------------', 'status');
    //MigrationBase::displayMessage(__METHOD__ . ' -AA- ' . var_dump($migration), 'status');

    if (!($ids[0])) {
      MigrationBase::displayMessage('FFFFFFFFFFFFFFFMMMMMMMMMMMMMMMMMMMM - FALSE ids[0] -', 'status');
      return FALSE;
    }

    // Create file_usage stub
    //  if ($migration->sourceValues->fid)
        //->type == 'file');
    $fm = new stdClass();
    $fm->fid = ''; // auto-increment
    $fm->uid = $ids[1];
    $fm->filename = 'stub';
    $fm->uri = 'stub';
    $fm->filemime = 'stub';
    $fm->filesize = 0;
    $fm->status = 0;
    $fm->timestamp = 0;
    $fm->type = 'stub';
    $fm->uuid = 'stub';
    $primary_keys = array('fid');
    drupal_write_record('file_managed', $fm, $primary_keys);
  }
}