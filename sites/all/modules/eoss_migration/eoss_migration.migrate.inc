<?php

/**
 * @file
 * this file is automatically loaded when hook_migrate_api is invoked by the
 * Migrate module.
 */

/*
 * Implement hook_migrate_api().
 */
function eoss_migration_migrate_api() {

  // Pre-set common arguments for creating classes
  $common_arguments = array(
    'source_connection' => 'eossMigrate',
    'source_version'=> '7',
    'format_mappings' => array(
      '1' => 'filtered_html',
    ),
  );
  // Each migration's custom arguments
  $users_arguments = $common_arguments + array(
    'class_name' => 'DrupalUser7MigrationEOSS',
    'machine_name' => 'DrupalUser7MigrationEOSS',
    'description' => t('Migration of webform submission user IDs.'),
    'group_name' => 'EOSSUsers',
  );
  $submissions_arguments = $common_arguments + array(
    'class_name' => 'MigrateWebformSubmissionEOSS',
    'machine_name' => 'MigrateWebformSubmissionEOSS',
    'description' => t('Migration of submissions from EOSS.asu.edu.'),
    'group_name' => 'EOSSSubmissions',
    'dependencies' => array('DrupalUser7MigrationEOSS'), // After users come in
  );
  $submitted_data_arguments = $common_arguments + array(
    'class_name' => 'MigrateWebformSubmittedDataEOSS',
    'machine_name' => 'MigrateWebformSubmittedDataEOSS',
    'description' => t('Migration of submitted data from EOSS.asu.edu.'),
    'group_name' => 'EOSSSubmissions',
    'dependencies' => array('MigrateWebformSubmissionEOSS'),
  );
  $file_managed_arguments = $common_arguments + array(
    'class_name' => 'MigrateWebformFileManagedEOSS',
    'machine_name' => 'MigrateWebformFileManagedEOSS',
    'description' => t('Migration of master information on webform submission files attached to file components.'),
    'group_name' => 'EOSSFiles',
    'dependencies' => array('MigrateWebformSubmissionEOSS'),
  );
  $file_usage_arguments = $common_arguments + array(
    'class_name' => 'MigrateWebformFileUsageEOSS',
    'machine_name' => 'MigrateWebformFileUsageEOSS',
    'description' => t('Migration of file usage webform submission files attached to file components.'),
    'group_name' => 'EOSSFiles',
    'dependencies' => array(
      'MigrateWebformSubmissionEOSS',
      'MigrateWebformFileManagedEOSS',
    ),
  );
  $last_download_arguments = $common_arguments + array(
    'class_name' => 'MigrateWebformLastDownloadEOSS',
    'machine_name' => 'MigrateWebformLastDownloadEOSS',
    'description' => t('Migration of last download of submission logs by EOSS.asu.edu webform administrators.'),
    'group_name' => 'EOSSSubmissions',
    'dependencies' => array(
      'DrupalUser7MigrationEOSS',
      'MigrateWebformSubmissionEOSS',
    ),
  );

  $api = array(
    'api' => 2,
    // destination handlers - Magically extends an existing destination and adds functionality
    // that the source may know nothing about.
    // Ex. MigrateCommentNodeHandler - adds comment fields to nodes when imported
    // from a source without any comments (if desired)
    'destination handlers' => array(
//      'DrupalDestinationUserEOSS',
//      'MigrateDestinationFileManagedEOSS',
    ),
    // Field Handlers - Manipulate individual fields for Drupal consumption on the destination end
    'field handlers' => array(
 //     'MigrateNameHandler',
    ),
    'migrations' => array(
      'DrupalUser7MigrationEOSS' => $users_arguments,
      'MigrateWebformLastDownloadEOSS' => $last_download_arguments,
      'MigrateWebformSubmissionEOSS' => $submissions_arguments,
      'MigrateWebformFileManagedEOSS' => $file_managed_arguments,
      'MigrateWebformFileUsageEOSS' => $file_usage_arguments,
      'MigrateWebformSubmittedDataEOSS' => $submitted_data_arguments,
    ),
    'groups' => array(
      'EOSSSubmissions' => array(
        'title' => t('EOSS Webform Submissions'),
      ),
      'EOSSUsers' => array(
        'title' => t('EOSS Webform Submission Users'),
      ),
      'EOSSFiles' => array(
        'title' => t('EOSS Webform Submitted Data - Files'),
      )
    ),
  );

  return $api;
}