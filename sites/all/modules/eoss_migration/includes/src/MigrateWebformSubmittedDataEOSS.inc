<?php

/**
 * Created by PhpStorm.
 * User: bryan
 * Date: 3/19/19
 * Time: 2:44 PM
 */
class MigrateWebformSubmittedDataEOSS extends DrupalMigration {

  /**
   * MigrateWebformSubmittedDataEOSS constructor.
   * @param array $arguments
   */
  function __construct(array $arguments) {
    parent::__construct($arguments);

    $table_name = 'webform_submitted_data';

    // @TODO - create new database col for primary key before migration?

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

    /**
     * Mappings
     */

    $this->addSimpleMappings($this->getFieldCols('simpleOnly'));

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
      'nid' => array(
        'description' => 'The node identifier of a webform.',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0,
      ),
      'sid' => array(
        'description' => 'The unique identifier for this submission.',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0,
      ),
      'cid' => array(
        'description' => 'The identifier for this component within this node, starts at 0 for each node.',
        'type' => 'int',
        'size' => 'small',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0,
      ),
      'no' => array(
        'description' => 'Usually this value is 0, but if a field has multiple values (such as a time or date), it may require multiple rows in the database.',
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'default' => '0',
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

    $what_fields = $this->getFieldCols('all');

    $wsd_query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_submitted_data', 'wsd');
    $wsd_query->fields('wsd', $what_fields)
      ->condition('wsd.sid', '331680', '>') // debugging condition - a dozen or so webform submissions
      ->orderBy('wsd.sid', 'ASC');
    return $wsd_query;
  }

  /**
   * Update cid to new cid.
   * @param $row
   * @return mixed
   */
  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // Grab new cid from matched (needs to be tested)
    $query = Database::getConnection('default')
      ->select(_eoss_webforms_followup_migration_data('table_cid'), 'mc')
      ->fields('mc', array('cid'))
      ->condition('mc.src_cid', $row->cid);
    $new_cid = $query->execute()->fetchField();
    $row->cid = $new_cid;

    // @TODO - Create stub for file_managed table below in prepareRow.

    $source_fid = $row->fid;

    MigrationBase::displayMessage(__METHOD__ . ' -- SRC File ID:' . $source_fid, 'status');

    $destination_fid = $this->handleSourceMigration(array(
      'MigrateWebformFileUsageEOSS',
    ),
      $source_fid);

    return $row;

  }

  function prepare() {
  }

  /**
   *
   * @param string $cols
   * - 'simpleOnly' - only returns fields that are Simple mapped (no keys)
   * - 'all' - returns all fields
   * @param string $associative - If yes, returns associative array with
   * key (col IDs) => value (field descriptions) pairs. Otherwise, simple array.
   *
   * @return array of webform_submission DB cols
   */
  public function getFieldCols($cols = 'simpleOnly', $associative = 'no') {
    $columns = new MigrateFieldsEOSS();
    $new = $columns->whichWebformDataCols($cols);
    if ($associative == 'no') { // return only keys as DB column names
      return array_keys($new);
    }
    else {
      return $new;
    }
  }
}