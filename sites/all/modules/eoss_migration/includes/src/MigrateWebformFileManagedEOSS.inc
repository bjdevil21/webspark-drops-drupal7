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


    ////////////////////
    // @TODO - Create stubs for file_usage tables
    // https://www.drupal.org/node/1013506
    ////////////////////

    $source_fid = $row->fid;

    MigrationBase::displayMessage(__METHOD__ . ' -- SRC File ID:' . $source_fid, 'status');

    $destination_fid = $this->handleSourceMigration(array(
      'MigrateWebformFileUsageEOSS',
    ),
      $source_fid);

    /**
     * @TODO may have to break out this into two to do a "stub from a stub" patch for file_usage
     */

    MigrationBase::displayMessage('WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS', 'status');
    MigrationBase::displayMessage(__METHOD__ . ' -- DST User ID:' . var_dump($destination_fid), 'status');
    MigrationBase::displayMessage('WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS', 'status');
    ///////////////////
    // END STUBS code
    ///////////////////

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

}