<?php

/**
 * Helper Library
 * List of DB columns to be added/removed from columns to be migrated, one
 * method per migration source class.
 */

class MigrateFieldsEOSS {

  /**
   * MigrateFieldsEOSS constructor.
   *
   * @return (array) default list of commonly used columns.
   */
  public function __construct() {
    $this->commonFields = array(
      'uid' => t('The uid of the submitter'),
      'nid' => t('The nid of the webform'),
      'sid' => t('The submission ID'),
    );
  }

  /**
   * @param string $what_columns
   * @return (array) webform_submission DB table columns
   */
  public function whichWebformSubmissionCols($column_set = 'all') {
    // Used cols instead of fields to avoid method name overlapping.
    $simple_cols = array(
      'is_draft' => t('Is this submission not completed?'),
      'submitted' => t('Timestamp of when the form was submitted'),
      'remote_addr' => t('The IP address of the user that submitted the form'),
      // EOSS additions
      'uuid' => t('The unique webform submission UUID'),
      'serial' => t('The serial ID of the submission; like a primary key'),
      'completed' => t('Timestamp of when submission ID was completed'),
      'highest_valid_page' => t(''),
      'modified' => t('Timestamp of when submission ID was last modified'),
    );
    if ($column_set == 'simpleOnly') {
      return $simple_cols;
    }
    else {
      return (array) array_merge($simple_cols, $this->commonFields);
    }
  }

  /**
   * @param string $what_columns
   * @return (array) webform_submitted_data DB table columns
   */
  public function whichWebformDataCols($column_set = 'all') {
    // Used cols instead of fields to avoid method name overlapping.
    $simple_cols = array(
      'no' => 'Usually 0 or 1',
      'data' => 'The submitted values',
    );
    if ($column_set == 'simpleOnly') {
      return $simple_cols;
    }
    else {
      $this->commonFields['cid'] = t('The webform component ID.');
      unset($this->commonFields['uid']);
      return (array) array_merge($simple_cols, $this->commonFields);
    }
  }

  /**
   * @param string $column_set - Return all or just simple columns list.
   * @return (array) file_managed DB table columns
   */
  public function whichFileManagedCols($column_set = 'all') {
    // Used cols instead of fields to avoid method name overlapping.
    $simple_cols = array(
      'filename' => t('The filename'),
      'uri' => t('The unprocessed URI to the file'),
      'filemime' => t('The MIME type of the file'),
      'filesize' => t('The size of the file (in bytes)'),
      'status' => t('Status of file to Drupal core - temporary (0) or permanent (1)'),
      'timestamp' => t('The upload timestamp'),
      'type' => t('The file type'),
      'uuid' => t('A unique hash ID (UUID) for the file'),
    );

    if ($column_set == 'simpleOnly') {
      return $simple_cols;
    }
    else {
      // fileManaged
      unset($this->commonFields['sid']);
      unset($this->commonFields['nid']);
      $this->commonFields['fid'] = t('The file ID');
      return (array) array_merge($simple_cols, $this->commonFields);
    }
  }

  /**
   * @param string $column_set - Return all or just simple columns list.
   * @return (array) webform_last_download DB table columns
   */
  public function whichWebformLastDownloadCols($column_set = 'all') {
    $simple_cols = array (
      'requested' => t('Timestamp of last download requested'),
    );
    if ($column_set == 'simpleOnly') {
      return $simple_cols;
    }
    else {
      return (array) array_merge($simple_cols, $this->commonFields);
    }
  }

  /**
   * Not currently being used.
   *
   * @param string $column_set - Return all or just simple columns list.
   * @return (array) file_usage DB table columns
   */
  public function whichFileUsageCols($column_set = 'all') {
    $simple_cols = array (
      'module' => t('Will be string webform'),
      'type' => t('will be string submission'),
      'count' => t('will be 1 for all webform submision records'),
    );
    if ($column_set == 'simpleOnly') {
      return $simple_cols;
    }
    else {
      // File Usage
      unset($this->commonFields['sid']);
      unset($this->commonFields['nid']);
      unset($this->commonFields['uid']);
      $this->commonFields['fid'] = t('The file ID');
      $this->commonFields['id'] = t('The webform submission ID');
      return (array) array_merge($simple_cols, $this->commonFields);
    }
  }
}
