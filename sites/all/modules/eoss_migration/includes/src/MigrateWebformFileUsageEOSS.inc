<?php

/**
 * Created by PhpStorm.
 * User: bryan
 * Date: 3/20/19
 * Time: 8:38 AM
 */
class MigrateWebformFileUsageEOSS extends DrupalMigration  {

  /**
   * MigrateWebformsFileUsageEOSS constructor.
   * @param array $arguments
   */
  public function __construct($arguments) {
    parent::__construct($arguments);

    $table_name = 'file_usage';

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
//      ->sourceMigration('')
//    ->createStub();

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
        'alias' => 'fu',
      ),
    );

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $mappable_key,
      MigrateDestinationTable::getKeySchema($table_name)
    );
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
  }

  /**
   * @return \SelectQueryInterface
   */
  public function query() {
    // TODO: Implement query() method.
    $fu_query = Database::getConnection('default', $this->sourceConnection)
      ->select('file_usage', 'fu');
    $fu_query->fields('fu', $this->getFieldCols('all'))
      ->orderBy('fu.fid');
    return $fu_query;
  }

  /**
   * @param $migration - Object that has migration data
   * @param $fid - new ID to inject
   */
  public function createStub($migration, array $fid) {
    // Create file_usage stub
    $fu = new stdClass();
    $fu->fid = $fid;
    $fu->id = '';
    $fu->module = 'webform';
    $fu->type = 'submission';
    $fu->count = 1;
    $primary_keys = array('fid');
      drupal_write_record('file_usage', $fu, $primary_keys);
  }

  /**
   * @param string $cols
   * @return array of webform_submission DB cols
   */
  public function getFieldCols($cols = 'all') {
    $columns = new MigrateWebformSubmissionFields();
    $new = $columns->whichFileCols($cols);
    return $new;
  }
}