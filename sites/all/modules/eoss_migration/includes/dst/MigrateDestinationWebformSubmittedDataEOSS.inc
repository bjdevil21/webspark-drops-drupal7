<?php

/**
 * Delete the orphaned data table data after completion of rollBack.
 */
class MigrateDestinationWebformSubmittedDataEOSS extends MigrateDestinationTable {
  public function postRollback() {
    $migration = Migration::currentMigration();
    // Remove DB entries for bad submitted data, based on $ids
    $table = 'migrate_map_custom_orphaned_ws_data';
    $query2 = Database::getConnection('default')
      ->schema()->tableExists($table);
    if ($query2) {
      db_delete('migrate_map_custom_orphaned_ws_data')
        ->execute();
      $migration->saveMessage('All orphaned data deleted from migrate_map_custom_orphaned_ws_data table.',
        'status');
    }
    else {
      $migration->saveMessage('Orphaned data migrate_map_custom_orphaned_ws_data table does not exist.',
        'error');
    }
  }
}