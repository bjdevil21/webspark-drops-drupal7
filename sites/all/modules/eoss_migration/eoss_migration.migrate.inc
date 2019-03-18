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

  $submissions_arguments = $common_arguments + array(
      'class_name' => 'MigrateWebformSubmissionEOSS',
      'machine_name' => 'MigrateWebformSubmissionEOSS',
      'description' => t('Migration of submissions from EOSS.asu.edu'),
      'group_name' => 'EOSSSubmissions',
      'dependencies' => array('DrupalUser7MigrationEOSS'), // After users come in
    );
  $submitted_data_arguments = $common_arguments + array(
      'class_name' => 'MigrateWebformSubmittedDataEOSS',
      'machine_name' => 'MigrateWebformSubmittedDataEOSS',
      'description' => t('Migration of submitted data from EOSS.asu.edu'),
      'group_name' => 'EOSSSubmissions',
      'dependencies' => array('MigrateWebformSubmissionEOSS'),
    );
  $last_download_arguments = $common_arguments + array(
      'class_name' => 'MigrateWebformLastDownloadEOSS',
      'machine_name' => 'MigrateWebformLastDownloadEOSS',
      'description' => t('Migration of last download of submission logs by EOSS.asu.edu webform administrators'),
      'group_name' => 'EOSSSubmissions',
      'dependencies' => array('MigrateWebformSubmissionEOSS'),
    );
  $users_arguments = $common_arguments + array(
      'class_name' => 'DrupalUser7MigrationEOSS',
      'machine_name' => 'DrupalUser7MigrationEOSS',
      'description' => t('Migration of webform submission user IDs'),
      'group_name' => 'EOSSUsers',
//      'soft_dependencies' => array(''), // After features + custom MySQL - which could've been done in Migrate API!!
    );
  $files_arguments = $common_arguments + array(
      'class_name' => 'MigrateSubmissionFilesEOSS',
      'machine_name' => 'MigrateSubmissionFilesEOSS',
      'description' => t('Migration of webform submission files'),
      'group_name' => 'EOSSFiles',
      'dependencies' => array('MigrateWebformSubmittedDataEOSS'),
    );

  $api = array(
    'api' => 2,
    // destination handlers - Magically extends an existing destination and adds functionality
    // that the source may know nothing about.
    // Ex. MigrateCommentNodeHandler - adds comment fields to nodes when imported
    // from a source without any comments (if desired)
    'destination handlers' => array(
 //     'DrupalDestinationUserEOSS',
    ),
    // Field Handlers - Manipulate the data for Drupal consumption -
    'field handlers' => array(
 //     'MigrateNameHandler',
    ),
    'migrations' => array(
      'MigrateDestinationWebformSubmissionEOSS' => $submissions_arguments,
//      'EOSSWebformSubmittedData' => $submitted_data_arguments,
//      'MigrateWebformLastDownloadEOSS' => $last_download_arguments,
      'DrupalUser7MigrationEOSS' => $users_arguments,
//      'MigrateSubmissionFilesEOSS' => $files_arguments,
    ),
    'groups' => array(
      'EOSSSubmissions' => array(
        'title' => t('EOSS Webform Submissions'),
      ),
      'EOSSUsers' => array(
        'title' => t('EOSS Webform Submission Users'),
      ),
      'EOSSFiles' => array(
        'title' => t('EOSS Webform Submitted Files'),
      )
    ),
  );

  return $api;
}