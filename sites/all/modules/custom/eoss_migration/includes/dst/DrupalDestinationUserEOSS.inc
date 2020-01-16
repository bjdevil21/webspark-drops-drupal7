<?php
/**
 * Migrate API destination class for user entity migration
 *
 * Works on moving the user entities (vs. raw DB records, but the following
 * DB tables are involved:
 *
 * - users
 * - users_roles
 *
 */

class DrupalDestinationUserEOSS extends DrupalDestinationUser {
  /**
   * @param null $migration
   * @return array
   */
  public function __construct(array $options = array()) {
    parent::__construct($options);
    $this->description =
      t('Custom destination for user data being migated by the DrupalUser7MigrationEOSS class.');
  }

  public function fields($migration = NULL) {
    $fields = parent::fields($migration);
    $fields['uuid'] = t('UUID');
    unset($fields['field_first_name:language']);
    unset($fields['field_last_name:language']);
    return $fields;
  }

  /**
   * Rolls back user IDs and hashes for skipped users
   * @param $entity_id
   */
  public function prepareRollback($entity_id) {

    $query = Database::getConnection('default', 'default')
      ->select('migrate_map_custom_skipped_users', 'msu')
      ->fields('msu')
      ->condition('msu.uid', $entity_id);
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
    // Return entry to original status before any rollback actions are taken
      $subquery = Database::getConnection('default', 'default')
        ->update('migrate_map_drupaluser7migrationeoss')
        ->fields(array(
          'destid1' => NULL,
          'needs_update' => 2,
          'hash' => NULL,
        ))
        ->condition('destid1', $result->uid);
      $subquery->execute();

      // Delete extra added user role for deleted user
      foreach ($entity_id as $new_uid) {
        db_delete('users_roles')
          ->condition('uid', $new_uid)
          ->condition('rid', 5)
          ->execute();
      }
    }
  }


  /**
   * Rolls back role IDs for imported users (roles were not brought over in the user import).
   * @param $entity_id
   */
  public function completeRollback($entity_id) {
    // Delete extra added user role for deleted user
    foreach ($entity_id as $new_uid) {
      db_delete('users_roles')
        ->condition('uid', $new_uid)
        ->condition('rid', 5)
        ->execute();
    }
  }

  /**
   * Need to declare this here to allow prepareRollback to work properly above
   * @param array $uids
   */
  public function bulkRollback(array $uids) {
    migrate_instrument_start('user_delete_multiple');
    $this->prepareRollback($uids);
    user_delete_multiple($uids);
    $this->completeRollback($uids);
    migrate_instrument_stop('user_delete_multiple');
  }
}