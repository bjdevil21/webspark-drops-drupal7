<?php

/**
 * Created by PhpStorm.
 * User: bryan
 * Date: 3/19/19
 * Time: 2:44 PM
 */
class MigrateWebformSubmttedDataEOSS extends DrupalMigration {

  /**
   * MigrateWebformSubmttedDataEOSS constructor.
   * @param array $arguments
   */
  function __construct(array $arguments) {
    parent::__construct($arguments);

    $table_name = 'webform_submitted_data';

    // @TODO - create new database col for primary key before migration?

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

    /**
     * Mappings
     */

    $this->addSimpleMappings($this->getFieldCols('simple'));

    // Component ID
    $this->addFieldMapping('cid', 'cid');
    // @TODO - prepareRow will do custom mapping in prepareRow

    // Node ID
    $this->addFieldMapping('nid', 'nid')
      ->sourceMigration('MigrateWebformSubmissionEOSS');

    // Submission ID
    $this->addFieldMapping('sid', 'sid')
      ->sourceMigration('MigrateWebformSubmissionEOSS');

    // @TODO -- Figure out why does this combination of DB cols work when none
    // of them alone are unique..
    // Adapted from Migrate Webform module's submitteddata.inc
    $mappable_keys = array(
      array(
        'nid' => array('type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Source node ID',
          'alias' => 'n',
        ),
        'sid' => array('type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Source submission ID',
          'alias' => 's',
        ),
        'cid' => array('type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Source component ID',
          'alias' => 'c',
        ),
        'no' => array('type' => 'varchar',
          'length' => 128,
          'not null' => TRUE,
          'description' => 'Source component instance ID?',
          'alias' => 'o',
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
    $wsd_query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_submitted_data', 'wsd');
    $wsd_query->fields('wsd', $this->getFieldCols('all'))
      ->condition('wsd.sid', '331690', '>') // debugging condition - a single webform submission
      ->orderBy('wsd.sid', 'ASC');
    return $wsd_query;
  }

  function prepareRow($row) {
    return parent::prepareRow($row);
    // @TODO - Create stub for file_usage table below in prepareRow.


  }

  function prepare() {
  }

  /**
   * @param string $cols
   * @return array of webform_submission DB cols
   */
  public function getFieldCols($cols = 'all') {
    $columns = new MigrateFieldsEOSS();
    $new = $columns->whichWebformDataCols($cols);
    return $new;
  }
}