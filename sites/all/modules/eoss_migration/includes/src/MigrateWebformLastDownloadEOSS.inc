<?php

/**
 * Created by PhpStorm.
 * User: bryan
 * Date: 3/21/19
 * Time: 5:37 AM
 */
class MigrateWebformLastDownloadEOSS extends DrupalMigration {

  function __construct(array $arguments) {
    parent::__construct($arguments);

    $table_name = 'webform_last_download';

    // @TODO - create new database col for primary key before migration?

    $this->source = new MigrateSourceSQL(
      $this->query(),
      $this->getFieldCols('all','yes'),
      NULL,
      array(
        'map_joinable' => FALSE,
        'skip_count' => FALSE
      )
    );

    $this->destination = new MigrateDestinationTable($table_name);

    // Mappings
    $this->addSimpleMappings($this->getFieldCols('simple'));
    // Node ID
    $this->addFieldMapping('nid', 'nid')
      ->sourceMigration('MigrateWebformSubmissionEOSS');

    // Submission ID
    $this->addFieldMapping('sid', 'sid')
      ->sourceMigration('MigrateWebformSubmissionEOSS');

    // Default uid to 0 if we're not mapping it.
    $this->removeFieldMapping('uid');
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('DrupalUser7MigrationEOSS');

    $mappable_keys = array(
      array(
        'nid' => array('type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Source node ID',
          'alias' => 'n',
        ),
      ),
    );

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $mappable_keys,
      MigrateDestinationTable::getKeySchema($table_name)
    );

  }

  // @TODO - Select query to grab webform_submitted_data (all of it)
  public function query() {
    $wld_query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_last_download', 'wld');
    $wld_query->fields('wld', $this->getFieldCols('all'))
      ->condition('wld.sid', '331680', '>') // debugging condition - a single webform submission
      ->orderBy('wld.sid', 'ASC');
    return $wld_query;
  }

  function prepareRow($row) {
    return parent::prepareRow($row);
  }

  function prepare() {
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
    $new = $columns->whichWebformLastDownloadCols($cols);
    if ($associative == 'no') { // return only keys as DB column names
      return array_keys($new);
    }
    else {
      return $new;
    }
  }
}