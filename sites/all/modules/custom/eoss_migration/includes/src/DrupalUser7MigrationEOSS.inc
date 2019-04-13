<?php
/**
 * @file
 * Implementation of
 * * Migrate d2d class extension for d7
 */

/**
 * Migrate API source class for user entity migration
 * Extends DrupalUser7Migration class (migrate_d2d module)
 *
 * DB tables are involved:
 *
 * - webform_submissions
 * - users
 * - users_roles
 */

class DrupalUser7MigrationEOSS extends DrupalUser7Migration {
  /**
   * EOSSSubmissionUsers constructor.
   * @param array $arguments
   */
  public function __construct(array $arguments) {
    // Add UUID field (sourceFields defined initially in parent class)
    $this->sourceFields['uuid'] = t('UUID');

    // Now call constructor with added field
    parent::__construct($arguments);

    $this->description =
      t('Migrates EOSS.asu.edu Drupal users that have submitted a webform or downloaded 
      submission reports to eoss-forms.asu.edu.');

    $this->addFieldMapping('uuid', 'uuid');
    $this->addFieldMapping('field_first_name', 'field_first_name');
    $this->addFieldMapping('field_last_name', 'field_last_name');

    $this->addUnmigratedSources(array(
      'field_user_about',
      'field_user_picture',
    ));
    $this->addUnmigratedDestinations(array(
      'field_user_about',
      'field_user_picture',
      'field_user_about:summary',
      'field_user_about:format',
      'field_user_picture:file_class',
      'field_user_picture:preserve_files',
      'field_user_picture:destination_dir',
      'field_user_picture:destination_file',
      'field_user_picture:file_replace',
      'field_user_picture:source_dir',
      'field_user_picture:urlencode',
      'field_user_picture:alt',
      'field_user_picture:title',
    ));

    // Extends DrupalDestinationUser()
    $this->destination = new DrupalDestinationUserEOSS();

  }

  /**
   * Query for the basic user data, but pare down the user list to return only
   * users that have an entry in the webform_submissions table.
   *
   * @return QueryConditionInterface
   */
  protected function query() {

    // Query 1 webform submission users
    $query = Database::getConnection('default', 'eossMigrate')
      ->select('users', 'u');
    $query->innerJoin('webform_submissions', 'ws', 'u.uid = ws.uid');
    $query->fields('u')
      ->condition('u.uid', 0, '>')
      ->groupBy('u.uid')
      ->orderBy('u.uid');

    return $query;
  }

  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    // Check for duplicate user names in destination DB and skip the insert or
    // update if it exists already.

    // UPDATE - It should've just SQL UPDATE VALUES on the user record (vs. create
    // a new one, but it instead created a new user with _\d on the end of the name.
    // Skipping update
    $query = Database::getConnection('default', 'default')
      ->select('users', 'u')
      ->fields('u', array('name'))
      ->condition('u.name', $row->name)
      ->orderBy('u.uid')
      ->execute()
      ->fetchAll();

    if (count($query) > 0) {
      // Insert into custom mapping table for postImport() insertion into user mapping table
      // @TODO - Add new UID later
      db_insert('migrate_map_custom_skipped_users')
        ->fields(array(
          'src_uid' => $row->uid,
          'name' => $row->name,
          // hash taken from hash() in source.inc
          'hash' => md5(serialize($row)),
        ))
        ->execute();
      // Log skip in messages table.
      $message = t('[SKIP-I] - Drupal user ID !name skipped because it already exists.',
        array(
          '!name' => $row->name,
        ));
      Migration::queueMessage($message, MigrationBase::MESSAGE_WARNING);
      return FALSE;
    }
    else {
      Migration::displayMessage('[SUCCESS-I] - Creating user ID ' . $row->name, MigrationBase::MESSAGE_WARNING);
    }
  }

  /**
   * Implementation of Migration::complete().
   * Insert CAS role (ID: 5) row into users_roles when user has been added
   *
   * @param $account
   * @param $row
   */
  public function complete($account, $row) {
    parent::complete($account, $row);

    $query = db_insert('users_roles')
    ->fields(array(
      'uid' => $account->uid,
      'rid' => 5,
    ));
    $query->execute();

  }

  /**
   * Get all skipped users that need a destination ID in migrate_map_drupaluser7migrationeoss
   * for all following user mappings
   */
  public function postImport() {
    parent::postImport();

    $query = Database::getConnection('default', 'default')
      ->select('migrate_map_custom_skipped_users', 'msu')
      ->fields('msu', array('src_uid', 'uid', 'name', 'hash'));
    $results = $query->execute()->fetchAll();

    $count = count($results);

    if ($count > 0) {

      $message = '[NOTICE-I] - ' . $count . ' skipped user IDs found. Attempting to update UID mapping tables...';
      Migration::queueMessage($message, MigrationBase::MESSAGE_NOTICE);

      var_dump(__METHOD__);

      foreach ($results as $result) {

        $sub_query = db_select('users', 'u')
          ->fields('u', array('uid'))
          ->condition('u.name', $result->name, 'LIKE');
        $sub_result = $sub_query->execute()->fetchField();

        db_update('migrate_map_drupaluser7migrationeoss')
          ->fields(array(
            'destid1' => $sub_result,
            'needs_update' => 0,
            'hash' => $result->hash,
          ))
          ->condition('sourceid1', $result->src_uid)
          ->execute();

        var_dump($result->src_uid);
        var_dump($sub_result);
        var_dump($result->name);
        var_dump($result->hash);

        $message = t('Drupal user ID !uid (!name) mapped to old UID !src_uid.',
          array(
            '!uid' => $sub_result,
            '!name' => $result->name,
            '!src_uid' => $result->src_uid,
          ));
        Migration::queueMessage($message, MigrationBase::MESSAGE_NOTICE);
        var_dump($message);
      }
    }
    else {
      var_dump('ELSE');
      Migration::queueMessage("[WARNING-I] - No skipped user IDs recorded.", MigrationBase::MESSAGE_WARNING);
    }
  }

  /**
   * Deletes all custom mapped IDs from custom table
   */
  protected function postRollback() {
    db_delete('migrate_map_custom_skipped_users')
      ->execute();
  }

}
