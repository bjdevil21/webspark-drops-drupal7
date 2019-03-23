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
    // @TODO - deal with mapping to fm
    $this->addFieldMapping('fid', 'fid');
//    ->sourceMigration('')
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
   * @param string $cols
   * @return array of webform_submission DB cols
   */
  public function getFieldCols($cols = 'all') {
    $columns = new MigrateFieldsEOSS();
    $new = $columns->whichFileUsageCols($cols);
    return $new;
  }

  /** @TODO -
   * Create stub for file_managed table
   *
   * @param $migration - Object that has migration data
   * @param $ids - new ID to inject
   */
  public function createStub($migration, $ids) {

    $migration = Migration::currentMigration();

    MigrationBase::displayMessage(__METHOD__ . ' -FU- ' . var_dump($ids), 'status');
    MigrationBase::displayMessage('FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU+++++++++++', 'status');
    //MigrationBase::displayMessage(__METHOD__ . ' -AA- ' . var_dump($migration), 'status');

    if (empty($ids[0])) {
      return FALSE;
      MigrationBase::displayMessage('FFFFuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu - FALSE ids[0] -', 'status');

    }

    // Create file_usage stub
    //  if ($migration->sourceValues->fid)
    //->type == 'file');
    $fu = new stdClass();
    $fu->fid = $ids[0];
    $fu->id = $ids[1]; // New submission ID -- either pass this in with $ids, or run a custom query here with source $fid?
    $fu->status = 1;
    $fu->module = 'webform';
    $fu->type = 'submission';

    $primary_keys = array('fid');
    if (drupal_write_record('file_usage', $fu, $primary_keys)) {
      MigrationBase::displayMessage(__METHOD__ . ' -- TRUE -- record ' . var_dump($fu) . ' written to file_usage.', 'status');

      return $fu;
      // @TODO file moving logic??
    }
    else {
      MigrationBase::displayMessage(__METHOD__ . ' -- No file_usage record writted. FALSE.', 'status');
      return FALSE;
    }
  }

}