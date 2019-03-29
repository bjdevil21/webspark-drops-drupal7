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

    $this->description = t('Migrates the file_usage table data for the 
      eoss-migrate -> eoss-forms site migration.');

    $table_name = 'file_usage';

    $this->source = new MigrateSourceSQL(
      $this->query(),
      $this->getFieldCols('all', 'yes'),
      NULL,
      array(
        'map_joinable' => FALSE,
        'skip_count' => FALSE
      )
    );

    $this->destination = new MigrateDestinationTable($table_name);

    $this->addSimpleMappings($this->getFieldCols('simpleOnly'));

    // file ID mapping
    // @TODO - deal with mapping to fm
    $this->addFieldMapping('fid', 'fid')
      ->sourceMigration('MigrateWebformFileManagedEOSS');

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
   *
   * @param string $cols
   * - 'simpleOnly' - only returns simple fields that are Simple mapped.
   * - 'all' - returns associative array with key (col IDs) => value (field descriptions) pairs
   * @return array of webform_submission DB cols
   */
  public function getFieldCols($cols = 'simpleOnly', $associative = 'no') {
    $columns = new MigrateFieldsEOSS();
    $new = $columns->whichFileUsageCols($cols);
    if ($associative == 'no') { // return only keys as DB column names
      return array_keys($new);
    }
    else {
      return $new;
    }
  }



  /** @TODO -
   * Create stub for file_managed table
   *
   * @param $migration - Object that has migration data
   * @param $ids - new ID to inject
   */
  public function createStub($migration, $id) {

    $migration = Migration::currentMigration();

    MigrationBase::displayMessage(__METHOD__ . ' -FU- ' . var_dump($id), 'status');
    MigrationBase::displayMessage('FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU+++++++++++', 'status');
    //MigrationBase::displayMessage(__METHOD__ . ' -AA- ' . var_dump($migration), 'status');

    if (empty($id)) {
      MigrationBase::displayMessage('FFFFFFFFFFFFFFuuuuuuuuuuuuuuuuuuuuuuu - FALSE id -', 'status');
      return FALSE;
    }

    // Create file_usage stub
    $fu = new stdClass();
    $fu->fid = '';
    $fu->id = ''; // New submission ID -- either pass this in with $ids, or run a custom query here with source $fid?
    $fu->status = 1;
    $fu->module = 'webform';
    $fu->type = 'submission';

    $dest_id = db_insert('file_usage', 'fu')
      ->fields('fu', array(
        'fid' => '',
        'id' => '',
        'status' => 1,
        'module' => 'webform',
        'type' => 'submission',
      ))
      ->execute();

    if ($dest_id) {
      MigrationBase::displayMessage(__METHOD__ . ' -- Record ' . var_dump($fu->fid) . ' written to file_usage.', 'status');

      // @TODO file moving logic??

      return $fu->fid;
    }
    else {
      MigrationBase::displayMessage(__METHOD__ . ' -- No file_usage record written.', 'status');
      return FALSE;
    }
  }

}