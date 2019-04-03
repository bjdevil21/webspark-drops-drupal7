<?php

/**
 * Created by PhpStorm.
 * User: bryan
 * Date: 3/19/19
 * Time: 2:49 PM
 */
class MigrateWebformFileManagedEOSS extends DrupalMigration {

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
    );

    $dst_schema = MigrateDestinationTable::getKeySchema($table_name);
    $dst_schema[] = $mappable_key['uid'];


    $this->map = new MigrateSQLMap(
      $this->machineName,
      $mappable_key,
      MigrateDestinationTable::getKeySchema($table_name)
    );

  }

  public function query() {
    $fm_query = Database::getConnection('default', $this->sourceConnection)
      ->select('file_managed', 'fm');
    $fm_query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
    $fm_query->fields('fm', $this->getFieldCols('all'))
      ->condition('fm.fid', '46600', '>') // debugging condition
      ->condition('fu.module', 'webform', 'LIKE')
      ->condition('fu.type', 'submission', 'LIKE')
      ->orderBy('fm.fid', 'ASC');
    return $fm_query;
  }

  /**
   * preImport()
   *
   * Before import begins - Four steps:
   * 1) Update each webform file component's directory
   * 2) Create destination directories based on webform data (one for each form)
   * #3, 4 in other classes
   */
  public function preImport() {
    parent::preImport();

    // Get preset directory names for all webforms
    $fileInfo = new MigrateWebformFilesLibrary;
    $fileInfo = $fileInfo->getDirectoryNames();

    // Get methods for messaging methods
    $migration = Migration::currentMigration();

    // Get webform UUIDs, NIDs, CIDs linked up by Features-generated machine name
    $query = Database::getConnection('default')
      ->select('migrate_map_custom_cid', 'mc');
    $query->innerJoin('webform_component', 'wc', 'wc.machine_name = mc.machine_name');
    $query->innerJoin('node', 'n', 'n.nid = mc.nid');
    $query->fields('mc', array('machine_name', 'nid', 'cid'))
      ->fields('wc', array('extra'))
      ->fields('n', array('uuid'))
      ->condition('wc.type', 'file')
      ->orderBy('mc.nid');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      try {
        $extra = unserialize($result->extra);
        if (is_array($extra)) {

          // #0) Get private/public setting
          $fileInfo->private = $extra['private'];

      // #1) Update each webform file component - QA
          $extra['directory'] = $fileInfo->{$result->uuid};
          $extra = serialize($extra);
          Database::getConnection('default', 'default')
            ->update('webform_component')
            ->fields(array(
              'extra' => $extra,
            ))
            ->condition('nid', $result->nid)
            ->condition('cid', $result->cid)
            ->execute();

      // #2... Create default directories for each file component's settings
          // NOTE: Other directories will be created later if needed.
          $directory = new MigrateWebformFilesLibrary($result->nid);

          $dstFolder = $directory->getAbsFileFolder('dst');
          $files_subdir = ($fileInfo->private == 1) ? MigrateWebformFilesLibrary::getPrivateFolder()
              : MigrateWebformFilesLibrary::getPublicFolder();
          $dstFolder .= $files_subdir;

          if (file_prepare_directory($dstFolder)) {
            $target = $dstFolder . '/' . $directory->singleDir;
            // Create record of folder creation (or existence)
            db_merge('migrate_map_custom_files_dirs')
              ->key(array(
                'directory' => $directory->singleDir,
              ))
              ->fields(array(
                'directory_full' => $target,
                'nid' => $result->nid,
                'cid' => $result->cid,
              ))
              ->execute();
            // Make new directory if doesn't exist
            if (!file_exists($target) && !is_dir($target)) {
              drupal_mkdir($target);
              MigrationBase::displayMessage('Directory ' . $target . ' created.',
                MigrationBase::MESSAGE_INFORMATIONAL);
            }
            else {
              MigrationBase::displayMessage('Directory ' . $target . ' already exists. Skipping.',
                MigrationBase::MESSAGE_INFORMATIONAL);
            }
          }
          else {
            $migration->saveMessage('Directory ' . $directory . ' not created.',
              MigrationBase::MESSAGE_ERROR);
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
    // Get sid from old DB + new DB mappings...
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('file_usage', 'fu')
      ->fields('fu', array('id'))
      ->condition('fu.fid', $row->fid);
    $sid_old = $query->execute()->fetchField();
    $query = Database::getConnection('default')
      ->select('migrate_map_migratewebformsubmissioneoss', 'mm')
      ->fields('mm', array('destid1'))
      ->condition('mm.sourceid1', $sid_old);
    $sid = $query->execute()->fetchField();

    variable_set('eoss_migration_current_submission_id', $sid);

    // Grab latest AUTO_INCREMENT data for fid update
    $query = Database::getConnection('default', 'informationSchema')
      ->select('tables', 't')
      ->condition('t.table_schema', 'eoss-forms', 'LIKE')
      ->condition('t.table_name', 'file_managed', 'LIKE');
    $query->addExpression('AUTO_INCREMENT');
    $fid = $query->execute()->fetchField();

    // CREATE STUB NOW...
    $fu_row = $this->handleSourceMigration(array('MigrateWebformFileUsageEOSS'), $fid);

  }

  /**
   * Update (or remove) the file ID from $entity (not $row), so the insert can create
   * the new record and map it properly - and create the new file_usage stub.
   *
   * @param $entity
   * @param $row
   */
  public function prepare($entity, $row) {

    // Grab latest AUTO_INCREMENT data for fid update
    $query = Database::getConnection('default', 'informationSchema')
      ->select('tables', 't')
      ->condition('t.table_schema', 'eoss-forms', 'LIKE')
      ->condition('t.table_name', 'file_managed', 'LIKE');
    $query->addExpression('AUTO_INCREMENT');
    $fid = $query->execute()->fetchField();

    if (!empty($fid)) {
      $entity->fid = $fid;

      // Then... 3) Update each file_managed uri in $entity
      $nid_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($row->fid, 'nid'); // src fid => dst nid
      $uri_result = MigrateWebformFilesLibrary::getFileInfoFromCustomMap($row->fid, 'src_uri'); // src fid => src URI

      if ($uri_result && $nid_result) { // Update the URI with new directory information
        $directory = new MigrateWebformFilesLibrary($nid_result->nid);
        // Same URI bases for now - won't stay that way if migration includes public -> private transition...
        $src_uri_base = MigrateWebformFilesLibrary::isUriPrivate($uri_result->uri, TRUE);
        $dst_uri_base = MigrateWebformFilesLibrary::isUriPrivate($uri_result->uri, TRUE);

        $src_pattern = '|^(p\w{5,6})\://' . $src_uri_base . '(.+)$|';
        $dst_pattern = '$1://' . $dst_uri_base . $directory->singleDir . '/$2';
        $entity->uri = preg_replace($src_pattern, $dst_pattern, $uri_result->uri);
      }
      else {
        MigrationBase::saveMessage('[FAIL-Prepare] nid_result or uri_result not set.', MIGRATION::MESSAGE_ERROR);
      }
    }
    else {
      MigrationBase::saveMessage('[FAIL-Prepare] Failed auto_increment check.', MIGRATION::MESSAGE_ERROR);
    }
    var_dump('stoppage');
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
