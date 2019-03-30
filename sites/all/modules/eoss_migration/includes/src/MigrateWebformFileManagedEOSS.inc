<?php

/**
 * Created by PhpStorm.
 * User: bryan
 * Date: 3/19/19
 * Time: 2:49 PM
 */
class MigrateWebformFileManagedEOSS extends DrupalMigration {

  protected $fileInfo;

  /**
   * MigrateWebformFileManagedEOSS constructor.
   * @param array $arguments
   */
  public function __construct($arguments) {
    parent::__construct($arguments);

    $this->description = t('Migrates file_managed table data (and their attached 
      files in the files directory) for the eoss-migrate -> eoss-forms site migration.');

    $table_name = 'file_managed';

    $this->source = new MigrateSourceSQL(
      $this->query(),
      $this->getFieldCols('all', 'yes'),
      NULL,
      array(
        'map_joinable' => FALSE,
        'skip_count' => FALSE
      )
    );

    $this->destination = new MigrateDestinationFileManagedEOSS($table_name);

    $this->addSimpleMappings($this->getFieldCols('simpleOnly'));

    // file ID mapping
    // @TODO - add in createStub to make file_usage stubs, or do file_usage first
    $this->addFieldMapping('fid', 'fid');

    // Default uid to 0 if we're not mapping it.
    $this->removeFieldMapping('uid');
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('DrupalUser7MigrationEOSS')
      ->defaultValue(0); // Anonymous user

    $mappable_key = array(
      'fid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'File ID',
        'alias' => 'fm',
      ),
      // Have to add uri for custom rollback changes
      'uri' => array(
        'description' => 'The URI to access the file (either local or remote).',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
        'binary' => TRUE,
      ),
    );
// Have to add uri for custom rollback changes
    $dst_schema = MigrateDestinationTable::getKeySchema($table_name);
    $dst_schema['uri'] = $mappable_key['uri'];

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $mappable_key,
//      MigrateDestinationTable::getKeySchema($table_name)
      $dst_schema
    );

    // NEW
    $this->fileInfo = new MigrateWebformFileDirectories();

  }

  public function query() {
    $fm_query = Database::getConnection('default', $this->sourceConnection)
      ->select('file_managed', 'fm');
    $fm_query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
    $fm_query->fields('fm', $this->getFieldCols('all'))
      // ->condition('fm.fid', '', '>') // debugging condition
      ->condition('fu.module', 'webform', 'LIKE')// All attached file entries
      ->condition('fu.type', 'submission', 'LIKE')
      ->orderBy('fm.fid', 'ASC');
    return $fm_query;
  }

  /**
   * preImport()
   *
   * Before import begins - Four steps:
   * 1) Update each webform file component
   * 2) Create destination directories based on webform data (one for each form)
   * #3, 4 in other classes
   */
  public function preImport() {
    parent::preImport();

    $migration = Migration::currentMigration();

    $query = Database::getConnection('default')
      ->select('migrate_map_custom_cid', 'mc');
    $query->innerJoin('webform_component', 'wc', 'wc.machine_name = mc.machine_name');
    $query->fields('mc', array('machine_name', 'nid', 'cid'))
      ->fields('wc', array('extra'))
      ->condition('wc.type', 'file')
      ->orderBy('mc.nid');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      try {
        $extra = unserialize($result->extra);
        if (is_array($extra)) {
      // #1... update each webform file component - QA
          $extra['directory'] = $this->fileInfo->getDirectoryNames()->{$result->machine_name};
          $extra = serialize($extra);
          db_update('webform_component', 'wc')
            ->fields(array(
              'extra' => $extra,
            ))
            ->condition('wc.nid', $result->nid)
            ->condition('wc.cid', $result->cid)
            ->execute();

      // #2... Create directories - __toString returns the single result - QA
          $directory = new MigrateWebformFileDirectories($result->nid);

          if (file_prepare_directory($this->fileInfo->getDstFolder())) {
            $target = $this->fileInfo->getDstFolder() . '/' . $directory;
            if (!file_exists($target) && !is_dir($target)) {
              drupal_mkdir($target);
            }
            else {
              $migration->saveMessage('Directory ' . $directory . ' already exists. Skipping.',
                MigrationBase::MESSAGE_NOTICE);
            }
          }
          unset($directory);

        }
      } catch (MigrateException $exception) {
        MigrationBase::displayMessage('The settings for the ' . $result->machine_name
          . ' webform component could not be retrieved and updated.');
      }
    }

  }

  public function prepareRow($row) {
    // Skip record validation
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    // 3) prepareRow will update each fm.uri, and

    /*
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('file_managed', 'fm');
    $query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
    $query->innerJoin('webform_submissions', 'ws', 'fu.id = ws.sid');
    $query->innerJoin('node', 'n', 'ws.nid = n.nid');
    $query->fields('fm', array('uri'))
      ->fields('n', array('machine_name'))
      ->isNotNull('ws.nid')
      ->condition('fm.uri', 'p%', 'LIKE')
      ->condition('fu.module', 'webform', 'LIKE')
      ->condition('fm.fid', $row->fid) // limited to one row by fid
      ->orderBy('ws.nid');
    $result = $query->execute()->fetchObject();
    */
    $result = $this->fileInfo->getInfoFromFid($row->fid);

    if ($result) {
      $dir = $this->fileInfo->getDirectoryNames()->{$result->machine_name};

      MigrationBase::displayMessage('fm_uri::before -- ' . $result->uri, MigrationBase::MESSAGE_INFORMATIONAL);
      MigrationBase::displayMessage('newDirectory -- ' . $dir, MigrationBase::MESSAGE_INFORMATIONAL);
      $fm_uri = preg_replace('|^(p\w{5,6})://webform/(.+)$|', "$1://webform/$dir/$2", $result->uri);
      // DEBUG --
      MigrationBase::displayMessage('fm_uri::after -- ' . $fm_uri, MigrationBase::MESSAGE_INFORMATIONAL);

      // Update the URI
      db_update('file_managed', 'fm')
        ->fields(array(
          'uri' => $fm_uri,
        ))
        ->condition('fm.fid', $result->fid)
        ->execute();
    }
  }

  public function prepare($entity, $row) {
    // Remove the old File ID from $entity (not $row),
    // so the insert can create the new record and map it properly.
    unset($entity->fid);
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
    $new = $columns->whichFileManagedCols($cols);
    if ($associative == 'no') { // return only keys as DB column names
      return array_keys($new);
    }
    else {
      return $new;
    }
  }
}
