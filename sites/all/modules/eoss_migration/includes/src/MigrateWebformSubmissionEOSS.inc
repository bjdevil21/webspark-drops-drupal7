<?php

/**
 * Migrate EOSS webform submissions into eoss-forms
 * Run this after
 */
class MigrateWebformSubmissionEOSS extends DrupalMigration {

  /**
   * MigrateWebformSubmissionEOSS constructor.
   * @param array $arguments
   */

  public function __construct($arguments) {
    parent::__construct($arguments);

    $table_name = 'webform_submissions';

    $this->source = new MigrateSourceSQL(
      $this->query(),
      $this->getFieldCols('all', 'yes'),
      NULL,
      array(
        'map_joinable' => FALSE,
        'skip_count' => FALSE
      )
    );

    // MigrateDestinationWebformSubmissionEOSS - Updated clone of Migrate Extra's webform.inc
    //   - MigrateDestinationWebformSubmission class
    //   - builds its own key functions and extends MigrateDestination
    // SELECTED -> MigrateDestinationTable - pass in the table name only, and see what gets mapped.
    $this->destination = new MigrateDestinationTable($table_name);

    $this->addSimpleMappings($this->getFieldCols('simpleOnly'));

    // Node ID mapping
    $this->addFieldMapping('nid', 'nid');
    // Features API was used for building the webform nodes already. See prepareRow() for update code.

    // Source ID mapping
    $this->addFieldMapping('sid', 'sid');

    // Default uid to 0 if we're not mapping it.
    $this->removeFieldMapping('uid');
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('DrupalUser7MigrationEOSS')
      ->defaultValue(0); // Anonymous user

    $mappable_key = array(
      'sid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'Source component ID',
        'alias' => 's',
      ),
    );

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $mappable_key,
      MigrateDestinationTable::getKeySchema($table_name)
    );
  }

protected function query() {
    $what_fields = $this->getFieldCols('all');

    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_submissions','ws')
      ->fields('ws', $what_fields);
  /*
    // DEBUGGING
    $db_or = db_or(); // Object
    //Returns a new DatabaseCondition, set to "OR" all conditions together.
    $subjids = array(
      '296057',
      '296060',
      '296065',
      '296069',
      '296075',
    );
    foreach($subjids as $subjid){
      $db_or->condition('wss.sid',$subjid,'=');
    }
    $query->condition($db_or);
    // END DEBUGGING
*/
    $query->orderBy('ws.sid', 'ASC');
    return $query;
  }


  /**
   * @param $row
   * @return bool
   */
  public function prepareRow($row) {

    // Manual sourceMigration() implementation for nids imported Features API; keyed by uuid.
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    else {
      $query = Database::getConnection('default')
        ->select('migrate_map_custom_nids', 'mm')
        ->fields('mm', array('nid'))
        ->condition('src_nid', $row->nid);
      $new_nid = $query->execute()->fetchField();

      if (!$new_nid) { // No matching webform in destination? Skip...
        MigrationBase::displayMessage('[SKIP] - Webform does not exist for submission ID ' . $row->sid, 'status');
        return FALSE;
      }
      else {
        $row->nid = (int) $new_nid;
      }
      if (empty($row->sid)) {
        return FALSE;
      }
      MigrationBase::displayMessage('Webform submission ID ' . $row->sid . ' from node ID ' . $row->nid . ' imported', 'status');
    }

    return $row;
  }

  public function prepare($entity, $row) {
    // Remove the old submission ID from $entity (not $row),
    // so the insert can create the new record and map it properly.
    unset($entity->sid);
  }

  /**
   * @param string $cols
   * @return array of webform_submission DB cols
   */
  public function getFieldCols($cols = 'simpleOnly', $associative = 'no') {
    $columns = new MigrateFieldsEOSS();
    $new = $columns->whichWebformSubmissionCols($cols);

    if ($associative == 'no') { // return only keys as DB column names
      return array_keys($new);
    }
    else {
      return $new;
    }
  }

}

