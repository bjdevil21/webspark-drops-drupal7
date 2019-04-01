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
      ->condition('fu.module', 'webform', 'LIKE')
      ->condition('fu.type', 'submission', 'LIKE')
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
  public function createStub($migration, $fid) {

    $migration = Migration::currentMigration();

    var_dump(__METHOD__);
    var_dump("-----------------");
    var_dump($fid);
    var_dump("-----------------");

    if (empty($fid)) {
      MigrationBase::displayMessage('---------------- [SKIP] No fid value. file_usage createStub skipped. -', MIGRATION::MESSAGE_ERROR);
      return FALSE;
    }

    // Create file_usage stub
    $fu = new stdClass();
    $fu->fid = $fid;
    $fu->id = ''; // New submission ID -- either run a custom query here with source $fid, or update on file_usage run
    $fu->status = 1;
    $fu->module = 'webform';
    $fu->type = 'submission';

    // Create or update if exists already
    $dest_id = db_merge('file_usage')
      ->key(array(
        'fid' => $fu->fid,
      ))
      ->fields(array(
        'fid' => $fu->fid,
        'id' => $fu->id,
        'status' => $fu->status,
        'module' => $fu->module,
        'type' => $fu->type,
      ))
      ->execute();

    if ($dest_id) {
      MigrationBase::displayMessage(__METHOD__ . ' -- Stub with fid '
        . var_dump($fu->fid) . ' updated/inserted to file_usage.', MIGRATION::MESSAGE_INFORMATIONAL);
      return $fu;
    }
    else {
      MigrationBase::displayMessage(__METHOD__ . ' -- No file_usage record written.', MIGRATION::MESSAGE_ERROR);
      return FALSE;
    }
  }

}