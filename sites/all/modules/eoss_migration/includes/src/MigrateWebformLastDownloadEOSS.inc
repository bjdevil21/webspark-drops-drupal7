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
    $this->addSimpleMappings($this->getFieldCols('simpleOnly'));

    // Node ID
    $this->addFieldMapping('nid', 'nid');
      // prepare() will need to manage this

    // Submission ID
    $this->addFieldMapping('sid', 'sid')
      ->sourceMigration('MigrateWebformSubmissionEOSS');

    // User ID
    $this->removeFieldMapping('uid');
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('DrupalUser7MigrationEOSS');

    $mappable_keys = array(
      'nid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0,
      ),
      'uid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0,
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
      ->select('webform_last_download', 'wld')
      ->fields('wld', $this->getFieldCols('all'))
      ->orderBy('wld.nid', 'ASC');
    return $wld_query;
  }

  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    MigrationBase::displayMessage(var_dump($row), 'status');
    if ($row->uid == 0) {
      MigrationBase::displayMessage('[SKIP] - Drupal user ID ' . $row->uid . ' records will not be imported.', 'status');
      return FALSE;
    }
  }

  /**
   * @param $entity
   * @param $row
   */
  function prepare($entity, $row) {
    // Grab new nid from matched (needs to be tested)
    $query = Database::getConnection('default')
      ->select(_eoss_webforms_followup_migration_data('table_nid'), 'mn')
      ->fields('mn', array('nid'))
      ->condition('mn.src_nid', $row->nid);
    $result = $query->execute()->fetch();
    if (!$result) {
      MigrationBase::displayMessage('[SKIP] - No matching destination webform for source node ID ' . $row->nid, 'status');
      return FALSE;
    }
    $entity->nid = $result->nid;

  }

  /**
   *
   * @param string $cols
   * - 'simpleOnly' - only returns simple fields that are Simple mapped.
   * - 'all' - returns simple + other fields as defined in MigrateFieldsEOSS class method
   * @param string $associative - return associative array with descriptions (yes) or just simple array of values (no).
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