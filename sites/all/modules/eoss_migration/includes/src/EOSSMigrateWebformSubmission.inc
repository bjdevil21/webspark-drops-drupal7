<?php

/**
 * Migrate EOSS webform submissions into eoss-forms
 * Run this after
 */
class EOSSMigrateWebformSubmission extends DrupalMigration {

  /**
   * EOSSMigrateWebformSubmission constructor.
   * @param array $arguments
   */
  public function __construct($arguments) {
    parent::__construct($arguments);

    $query = $this->query();

    $table_name = 'webform_submissions';

    $this->source = new MigrateSourceSQL(
      $query,
      $this->whichWebformDBCols('all'),
      NULL,
      array(
        'map_joinable' => FALSE,
        'skip_count' => FALSE
      )
    );

    $this->destination = new MigrateDestinationTable($table_name);

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

    $this->addSimpleMappings($this->whichWebformDBCols('simple'));

    // Node ID mapping
    $this->addFieldMapping('nid', 'nid');
    // This mapping will be done manually below in prepareRow because Features API
    // was used for building the webform nodes already.
    // ->sourceMigration($arguments['node_migrations']);

    // Default uid to 1 if we're not mapping it.
    $this->removeFieldMapping('uid');
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('EOSSDrupalUser7Migration')
      ->defaultValue(0); // Anonymous user
  }

  protected function query() {
    $what_fields = $this->whichWebformDBCols('all');
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_submissions', 'wss')
      ->fields('wss', $what_fields)
      ->condition('sid', '330000', '>') // debugging
      ->orderBy('sid', 'ASC');
    return $query;
  }

  protected function whichWebformDBCols($what_columns = 'all') {
    // Used cols instead of fields to avoid method name overlapping.
    $simple_cols = array(
      'completed', // Always the same as completed
      'highest_valid_page', // ALWAYS 0
      'is_draft', // ALWAYS 0
      'modified',
      'remote_addr',
      'sid',
      'serial', // This is the webform submission ID # for just THAT form, in order.
      'submitted', // Always the same as completed
      'uuid',
    );
    $complex_cols = array(
      'nid',
      'uid',
    );
    if ($what_columns == 'simple') {
      return $simple_cols;
    }
    else {
      return array_merge($simple_cols, $complex_cols);
    }
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
        ->condition('uuid', $uuid);
      $new_nid = $query->execute()->fetchField();
      if (!$new_nid) { // No matching webform in destination? Skip...
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
   *
   */
  public function getWebformNodeIDs() {

  }
}

