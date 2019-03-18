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
      $this->getWebformCols('all'),
      NULL,
      array(
        'map_joinable' => FALSE,
        'skip_count' => FALSE
      )
    );

    // @TODO - WHICH destination handler?
    //
    // MigrateDestinationWebformSubmissionEOSS - Updated clone of Migrate Extra's webform.inc
    //   - MigrateDestinationWebformSubmission class
    //   - builds its own key functions and extends MigrateDestination
    // MigrateDestinationTable - pass in the table name only, and see what gets mapped.
    //
    $this->destination = new MigrateDestinationTable($table_name);

    $this->addSimpleMappings($this->getWebformCols('simple'));

    // Node ID mapping
    $this->addFieldMapping('nid', 'nid');
    // This mapping will be done manually below in prepareRow because Features API
    // was used for building the webform nodes already.
    // ->sourceMigration($arguments['node_migrations']);

    // Default uid to 1 if we're not mapping it.
    $this->removeFieldMapping('uid');
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('DrupalUser7MigrationEOSS')
      ->defaultValue(0); // Anonymous user

    $this->map = new MigrateSQLMap(
      $this->machineName,
      array(
        'sid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Source component ID',
          'alias' => 's',
        ),
      ),
      MigrateDestinationTable::getKeySchema($table_name)
    );
  }

  protected function query() {
    $what_fields = array_keys($this->getWebformCols('all'));
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_submissions', 'wss')
      ->fields('wss', $what_fields)
      ->condition('wss.sid', '330000', '>') // debugging
      ->orderBy('wss.sid', 'ASC');
    return $query;
  }

  // whichWebformDBCols method moved out to separate class for global access.

  /**
   * @param $row
   * @return bool
   */
  public function prepareRow($row) {
    // Skip record validation
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // This checks if the destination nid exists for a particular webform before inserting the submission.
    /*
    $result = db_select("node", "n")
      ->fields("n", array("nid"))
      ->condition("nid", $row->nid)
      ->execute()
      ->fetchField();
    if (!$result) {
      return FALSE;
    }
    */

    // Manual nid mapping of nodes imported manually with features, by uuid.
    else {
      $query = Database::getConnection('default')
        ->select('node', 'n')
        ->fields('n', array('nid'))
        ->condition('nid', $old_nid);
      $new_nid = $query->execute()->fetchField();
      var_dump('row ID: ' . $row->nid);
      var_dump('new nid: ' . $new_nid);

      if (!$new_nid) { // No matching webform in destination? Skip...
        var_dump("no match");
        return FALSE;
      }
      else {
        $row->nid = (int) $new_nid;
      }
    }
  }

  public function prepare($entity, $row) {
    // No transforms necessary
  }
  /**
   * @param string $cols
   * @return array of webform_submission DB cols
   */
  public function getWebformCols($cols = 'all') {
    return MigrateWebformSubmissionFields::whichWebformDBCols($cols);
  }
}

