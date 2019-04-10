<?php

/**
 * Migration class for the webform_submitted_data table.
 *
 * DB Tables:
 * - webform_submitted_data
 * - webform_component
 * - migrate_map_migratewebformsubmissioneoss
 * - migrate_map_migratewebformfilemanagedeoss
 * - migrate_map_custom_nid
 * - migrate_map_custom_cid
 * - migrate_map_custom_orphaned_ws_data
 * - migrate_map_custom_files_dirs
 *
 */

class MigrateWebformSubmittedDataEOSS extends DrupalMigration {

  /**
   * Mapped cid, nid values
   * @var
   */
  protected $currentCid;
  protected $currentNid;
  protected $currentSid;

  /**
   * @return mixed
   */
  public function getCurrentCid() {
    return $this->currentCid;
  }
  /**
   * @param mixed $current_cid
   */
  public function setCurrentCid($currentCid) {
    $this->currentCid = $currentCid;
  }
  /**
   * @return mixed
   */
  public function getCurrentNid() {
    return $this->currentNid;
  }
  /**
   * @param mixed $currentNid
   */
  public function setCurrentNid($currentNid) {
    $this->currentNid = $currentNid;
  }
  /**
   * @return mixed
   */
  public function getCurrentSid() {
    return $this->currentSid;
  }
  /**
   * @param mixed $currentSid
   */
  public function setCurrentSid($currentSid) {
    $this->currentSid = $currentSid;
  }

  /**
   * MigrateWebformSubmittedDataEOSS constructor.
   * @param array $arguments
   */
  function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->description = t('Migrates the webform_submitted_data table data (holds the 
      individual data points for each submission) for the eoss-migrate -> eoss-forms 
      site migration.');

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

    $this->destination = new MigrateDestinationWebformSubmittedDataEOSS($table_name);

    /**
     * Mappings
     */

    $this->addSimpleMappings($this->getFieldCols('simpleOnly'));

    // Node ID
    $this->addFieldMapping('nid', 'nid');
    // prepareRow will do custom mapping

    // Submission ID
    $this->addFieldMapping('sid', 'sid')
      ->sourceMigration('MigrateWebformSubmissionEOSS');

    // Component ID
    $this->addFieldMapping('cid', 'cid');
    // prepareRow will do custom mapping

    // Because there are four source keys and no primary keys in the destination,
    // their values MUST be otherwise populated in $entity, in the prepare() function.

    $mappable_src_keys = array(
      'nid' => array(
        'description' => 'The node identifier of a webform.',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0,
      ),
      'sid' => array( // Must be first in this case, due to mapping in destination only mapping sid
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
      $mappable_src_keys,
      MigrateDestinationTable::getKeySchema($table_name)
    );

  }

  /**
   * @return \SelectQueryInterface
   */
  public function query() {
    $what_fields = $this->getFieldCols('all');

    $wsd_query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_submitted_data', 'wsd');
    $wsd_query->fields('wsd', $what_fields)
//      ->condition('wsd.sid', '331247', '>') // debugging condition
//      ->condition('wsd.sid', '331275', '<') // debugging condition
      ->condition('wsd.nid', '0', '>')
      ->condition('wsd.cid', '0', '>')
      ->orderBy('wsd.sid', 'ASC');
    return $wsd_query;
  }

  /**
   * Drop records if cid or nid empty/null/zero.
   * Populate skipped data in separate table.
   *
   * @param $row - src DB row
   * @return mixed
   */
  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // Skip if no src nid or src cid.
    foreach ($row as $property => $value) {
      if ($property == 'nid' || $property == 'cid' || $property == 'sid') {
        if ($value == 0 || is_null($value) || empty($value)) {
          $migration = Migration::currentMigration();
          $migration->queueMessage('[SKIP] - ' . $property . ':::' . $value
            . ' not set. Skipping.', Migration::MESSAGE_NOTICE);
          return FALSE;
        }
      }
    }

    // Skip if:
    // * SRC cid was valid but is no longer in the webform (due to form edits).
    // * SRC sid has no mapped destination
    // * SRC sid == 0

    $this->setCurrentSid(0); // ...assume bad SID as default locally
    $this->setCurrentCid(0);
    $this->setCurrentNid(0);

    // Check for existing nid + cid
    $query = Database::getConnection()
      ->select(_eoss_webforms_followup_migration_data('table_cid'), 'mc')
      ->fields('mc', array('cid', 'nid'))
      ->condition('mc.src_cid', $row->cid)
      ->condition('mc.src_nid', $row->nid);
    $result = $query->execute()->fetch();

    // IF there's a valid nid + cid, check for valid sid
    if ($result) {
      $this->setCurrentCid($result->cid);
      $this->setCurrentNid($result->nid);
      $query2 = Database::getConnection('default')
       ->select('migrate_map_migratewebformsubmissioneoss', 'mws')
       ->fields('mws', array('destid1'))
       ->condition('mws.sourceid1', $row->sid);
      $result2 = $query2->execute()->fetchAll();
      if (count($result2) === 1) {
        foreach ($result2 as $result_sid) {
          $this->setCurrentSid($result_sid->destid1);
        }
      }
    }

    // SKIP if no valid nid, cid, or sid
    if (!$result || $this->currentSid == 0) {
      $migration = Migration::currentMigration();
      $message = "Source SID# " . $row->sid . ' saved in migrate_map_custom_orphaned_ws_data table.';
      $migration->queueMessage($message, Migration::MESSAGE_WARNING);
      // save the data in migrate_map_orphaned_submitted_data table for later perusal (just in case).
      db_insert('migrate_map_custom_orphaned_ws_data')
        ->fields(array(
          'src_nid' => $row->nid,
          'src_cid' => $row->cid,
          'src_sid' => $row->sid,
          'no' => $row->no,
          'data' => $row->data,
          'nid' => $this->currentNid,
          'cid' => $this->currentCid,
          'sid' => $this->currentSid,
        ))
        ->execute();
      return FALSE;
    }
  }

  /**
   * @param $entity
   * @param $row
   */
  public function prepare($entity, $row) {
    // Grab new cid and nid from custom migration table
    $query = Database::getConnection('default')
      ->select(_eoss_webforms_followup_migration_data('table_cid'), 'mc')
      ->fields('mc', array('cid', 'nid'))
      ->condition('mc.src_cid', $row->cid)
      ->condition('mc.src_nid', $row->nid);
    $result = $query->execute()->fetch();
    $entity->cid = $result->cid;
    $entity->nid = $result->nid;

    // Transform file IDs in 'data' column via file_managed mapping
    if (preg_match('|^\s*?\d{2,6}\s*?$|', $row->data)) { // lowest fid on eossMigrate > 20
      $type = Database::getConnection()
        ->select('webform_component', 'wc')
        ->fields('wc', array('type'))
        ->condition('wc.cid', $entity->cid)
        ->condition('wc.nid', $entity->nid)
        ->execute()->fetchField();
      if ($type == 'file') {
        $query = Database::getConnection()
          ->select('migrate_map_migratewebformfilemanagedeoss', 'mfm')
          ->fields('mfm', array('destid1'))
          ->condition('mfm.sourceid1', trim($row->data));
        $results = $query->execute()->fetchAll();
        if (count($results) == 1) {
          foreach ($results as $result) {
            $entity->data = $result->destid1;
          }
        }
        else {
          MigrationBase::displayMessage('The wsd.data value ' . $entity->data
            . ' has no matching source fid in the file_managed mapping table.', 'warning');
        }
      } else {
        // Not a 'file' webform component
      }
    } else {
      // Not a number
    }
    // @TODO - Do select components need any transformation?
  }

  /**
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