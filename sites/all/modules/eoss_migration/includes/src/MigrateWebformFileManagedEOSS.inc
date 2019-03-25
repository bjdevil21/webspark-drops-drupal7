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

  public function query() {
    $fm_query = Database::getConnection('default', $this->sourceConnection)
      ->select('file_managed', 'fm');
    $fm_query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
    $fm_query->fields('fm', $this->getFieldCols('all'))
      // ->condition('fm.fid', '', '>') // debugging condition
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
  }

  public function prepare($entity, $row) {
    // Remove the old File ID from $entity (not $row),
    // so the insert can create the new record and map it properly.
    unset($entity->fid);

    // Now do file
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
    $new = $columns->whichFileManagedCols($cols);
    if ($associative == 'no') { // return only keys as DB column names
      return array_keys($new);
    }
    else {
      return $new;
    }
  }

}