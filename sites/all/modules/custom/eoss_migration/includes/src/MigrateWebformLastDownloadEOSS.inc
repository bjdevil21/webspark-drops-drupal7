<?php

/**
 * Migrate API source class for last download data.
 *
 * DB tables:
 *
 * - webform_last_download
 * - migrate_map_custom_nid
 * - migrate_map_drupaluser7migrationeoss
 *
 */

class MigrateWebformLastDownloadEOSS extends DrupalMigration {

  protected $customMapTable = 'migrate_map_custom_nid';

  public function getCustomMapTable() {
    return $this->customMapTable;
  }

  public function setCustomMapTable($table) {
    $this->customMapTable = _eoss_webforms_followup_migration_data($table);
  }

  /**
   * @var
   */
  protected $customMappings = '';

  public function getFileInfoFromCustomMaps() {
    return $this->customMappings;
  }

  public function setCustomMappings(stdClass $mappings) {
    $this->customMappings = $mappings;
  }

  /**
   * MigrateWebformLastDownloadEOSS constructor.
   * @param array $arguments
   */
  function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->description = t('Migrates the webform_last_download table data for the 
      eoss-migrate -> eoss-forms site migration.');

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
    $this->addFieldMapping('uid', 'uid');
    // prepare()

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

    $this->customMappings = new stdClass();

  }

  // @TODO - Select query to grab webform_submitted_data (all of it)
  public function query() {
    $wld_query = Database::getConnection('default', $this->sourceConnection)
      ->select('webform_last_download', 'wld')
      ->fields('wld', $this->getFieldCols('all'))
      ->orderBy('wld.nid', 'ASC');
    return $wld_query;
  }

  /**
   * Add new NIDs, UIDs from custom mapping tables
   * @param $row
   * @return bool
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    $query = Database::getConnection('default')
      ->select($this->customMapTable, 'mn')
      ->fields('mn', array('nid'))
      ->condition('mn.src_nid', $row->nid);
    $new_nid = $query->execute()->fetchField();

    $query = Database::getConnection('default')
      ->select('migrate_map_drupaluser7migrationeoss', 'mu')
      ->fields('mu', array('destid1'))
      ->condition('mu.sourceid1', $row->uid);
    $new_uid = $query->execute()->fetchField();

    if (is_null($new_nid) || empty($new_nid)) {
      $this->queueMessage('No webform matching webform source ID ' . $row->nid, MigrationBase::MESSAGE_NOTICE);
      return FALSE;
    }
    else {
      $this->customMappings->newNid = (int) $new_nid;
    }
    if (empty($new_uid) || is_null($new_uid)) {
      $this->queueMessage('Drupal user ID ' . $row->uid . ' cannot be found, so their last download record will not be imported.', MigrationBase::MESSAGE_NOTICE);
      return FALSE;
    }
    else {
      $this->customMappings->newUid = (int) $new_uid;
    }
  }

  /**
   * Plug new nid, uid from custom mapping into $entity
   * @param $entity
   * @param $row
   */
  function prepare($entity, $row) {
      $entity->nid = $this->customMappings->newNid;
      $entity->uid = $this->customMappings->newUid;
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