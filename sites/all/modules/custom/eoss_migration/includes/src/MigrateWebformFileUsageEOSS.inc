<?php

/** UPDATE
 * This class will never need to be called explicitly. Only the createStub method
 * is being utilized (during the file_managed migration), and the resulting stubs
 * have all of the data (meaning they're not really stubs at all).
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

    // Submission ID mapping
    $this->addFieldMapping('id', 'id')
      ->defaultValue(0); // no submission ID --

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
    // @TODO - get webform submission ID from map
    // (different column labels prohibit using the existing data in migrate_map from webform_submissions
    $query = Database::getConnection('default', 'default')
      ->select('migrate_map_migratewebformsubmissioneoss', 'mw')
      ->fields('mw', array('destid1'))
      ->condition('mw.sourceid1', $row->id)
      ->orderBy('mw.sourceid1');
    $result = $query->execute()->fetchField();

    if ($result) {
      $row->id = $result;
    }
    else {
      $this->queueMessage('No submission ID matches this file_usage entry. Skipping.', MIGRATION::MESSAGE_ERROR);
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
   * Create stubs for file_managed migration
   *
   * @param $migration - Object that has migration data
   * @param $ids - new ID to inject
   */
  public function createStub($migration, $fid) {

    $migration = Migration::currentMigration();
    $sid = variable_get('eoss_migration_current_submission_id', 0);

    if (empty($fid) || empty($sid)) {
      return FALSE;
    }

    // Create file_usage stub
    $fu = array(
      'fid' => (int) $fid[0],
      'id' => (int) $sid, // hack insertion method
      'module' => 'webform',
      'type' => 'submission',
      'count' => 1,
    );

    // Create or update if exists already
    db_insert('file_usage')
      ->fields($fu)
      ->execute();

    $migration->queueMessage('File ID ' . $fu['fid']. ' added to file_usage table as stub.', MIGRATION::MESSAGE_INFORMATIONAL);
    return array($fu['fid']);
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

}