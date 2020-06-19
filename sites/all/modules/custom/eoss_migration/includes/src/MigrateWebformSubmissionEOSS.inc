<?php

/**
 * Migrate API source class for webform submissions
 *
 *
 * DB tables are involved:
 *
 * - webform_submissions
 * - migrate_map_custom_nid
 *
 */
class MigrateWebformSubmissionEOSS extends DrupalMigration {

  /**
   * MigrateWebformSubmissionEOSS constructor.
   * @param array $arguments
   */

  public function __construct($arguments) {
    parent::__construct($arguments);

    $this->description = t('Migrates the webform_submissions table data (holds the 
    main webform submission records) for the eoss-migrate -> eoss-forms site migration.');

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

    $this->destination = new MigrateDestinationTable($table_name);

    $this->addSimpleMappings($this->getFieldCols('simpleOnly'));

    // Node ID mapping
    $this->addFieldMapping('nid', 'nid');
    // sourceMigration values taken care of in prepareRow() SQL logic because
    // Features API was used for building the webform nodes already.

    // Submission ID mapping
    $this->addFieldMapping('sid', 'sid');

    // User ID mapping -- Default uid to 0 if we're not mapping it.
    $this->removeFieldMapping('uid');
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('DrupalUser7MigrationEOSS')
      ->defaultValue(0); // Anonymous user

    $mappable_keys = array(
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
      $mappable_keys,
      MigrateDestinationTable::getKeySchema($table_name)
    );
  }

  protected function query() {
    $what_fields = $this->getFieldCols('all');

    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_submissions','ws')
      ->fields('ws', $what_fields)
      ->orderBy('ws.sid', 'ASC');
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
        ->select('migrate_map_custom_nid', 'mm')
        ->fields('mm', array('nid'))
        ->condition('src_nid', $row->nid);
      $new_nid = $query->execute()->fetchField();

      $old_nid = $row->nid;
      if (!$new_nid) { // No matching webform in destination? Skip...
        MigrationBase::saveMessage('[SKIP] - Webform does not exist for submission ID ' . $row->sid, 'status');
        return FALSE;
      }
      else {
        $row->nid = (int) $new_nid;
      }
      if (empty($row->sid)) {
        return FALSE;
      }
      MigrationBase::displayMessage('Importing old submission ID# ' . $row->sid
        . ' from old NID ' . $old_nid . ' into new NID ' . $row->nid, 'status');
    }

    return $row;
  }

  /**
   * Remove the old submission ID from $entity (not $row) so the insert can
   * create the new record and map it properly.
   *
   * @param $entity
   * @param $row
   */
  public function prepare($entity, $row) {
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
