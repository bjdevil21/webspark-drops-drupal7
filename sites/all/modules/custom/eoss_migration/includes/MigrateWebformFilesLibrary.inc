<?php

/**
 * Library of functions related to some of the EOSS migration classes.
 */
class MigrateWebformFilesLibrary {

  /**
   * Directories for file migration (src, dest)
   *
   * @var (bool) $is_private - If TRUE, return default files
   */
  private static $srcAbsFileFolder = '/home/bryan/eoss-migrate/sites/default/files/';
  private static $dstAbsFileFolder = DRUPAL_ROOT . '/sites/default/files/';
  private static $privateFolder = "private/webform/";
  private static $publicFolder = "webform/";
  // @TODO Use variable_get('file_public_path'), etc. if possible

  /**
   *
   * @return string - Absolute file directory for OS (private or public)
   */
  public static function getAbsFileFolder($mig_side): string {
    return ($mig_side == 'dst') ? self::$dstAbsFileFolder : self::$srcAbsFileFolder;
  }
  /**
   * @return string
   */
  public static function getPrivateFolder(): string {
    return self::$privateFolder;
  }
  /**
   * @return string
   */
  public static function getPublicFolder(): string {
    return self::$publicFolder;
  }

  /**
   * @var (obj upon instantiation) List of Node UUID-based directory names
   * (Does not include the entire file path)
   */
  protected $directoryNames;
  public function getDirectoryNames() {
    return $this->directoryNames;
  }

  /**
   * MigrateWebformFileDirectories constructor. Builds default directory names list upon each instantiation.
   * @param $node_id
   * @return string (or object if node_id not set) files directory
   */
  public function __construct($node_id = 0) {

    // INIT directoryNames variable
    $this->directoryNames = new stdClass();

    // Get webform nodes info for directory name creation
    $query = Database::getConnection('default', 'default')
      ->select('webform_component', 'wc');
    $query->innerJoin('node', 'n', 'wc.nid = n.nid');
    $query->fields('n', array('nid', 'title', 'uuid'))
      ->condition('n.type', 'webform', 'LIKE');
    // return only single node title
    if ($node_id > 0) {
      $query->condition('n.nid', $node_id);
    }
    $query->groupBy('n.nid'); // Return only one result per nid, since they're all the same across nids.
    $query->orderBy('n.nid');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      $title = substr($result->title, 0, 20);
      $this->directoryNames->{$result->uuid} = $result->nid . '__' . urlencode(str_replace(' ', '-', $title));
    }
    if (count($results) === 1) { // set special property for single output
      $this->singleDir = $this->directoryNames->{$result->uuid};
    }
    else {
      $this->singleDir = "";
    }
  }

  /**
   * Return various file_managed records (based on file_managed.fid) for better directory structure
   * creation for Webform-submitted files during import/rollback.
   *
   * Needs SRC database (eoss-migrate) to work
   *
   * @param string $fid - file ID from either file_managed or file_usage DB tables
   * @return bool if FALSE, else object $result
   */
  public static function getFileInfoFromCustomMap($fid, $data_type = 'nid') {

    // Option 1: (depending on data_type)
    //// nid: Old FID => New NID
    //// n_uuid: Old FID => Old UUID (same as New UUID)
    if ($data_type == 'nid' || $data_type == 'n_uuid') {
      $data_point = ($data_type == 'nid') ? 'nid' : 'uuid';
      $query = Database::getConnection('default', 'eossMigrate')
        ->select('file_managed', 'fm');
      $query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
      $query->innerJoin('webform_submissions', 'ws', 'fu.id = ws.sid');
      $query->innerJoin('node', 'n', 'ws.nid = n.nid');
      $query->fields('fm', array('fid', 'uri'))
        ->fields('n', array('nid', 'uuid'))
        ->isNotNull('ws.nid')
        ->condition('fm.uri', 'p%', 'LIKE')
        ->condition('fu.module', 'webform', 'LIKE')
        ->condition('fm.fid', $fid)
        ->groupBy('n.nid')
        ->orderBy('ws.nid');
      $results = $query->execute()->fetchAll();
      //// Part 2 - Old NID => New NID
      if (count($results) == 1) {
        $query2 = Database::getConnection('default', 'default')
          ->select('migrate_map_custom_nid', 'mn')
          ->fields('mn', array($data_point))
          ->condition('mn.src_nid', $results[0]->nid);
        $results = $query2->execute()->fetchAll();
      }
    }

    // Option 2 - Old FID => Old URI
    if ($data_type == 'src_uri') {
      $query = Database::getConnection('default', 'eossMigrate')
        ->select('file_managed', 'fm')
        ->fields('fm', array('uri'))
        ->condition('fm.fid', $fid);
      $results = $query->execute()->fetchAll();
    }

    // Option 3 - New FID => Old FID data
    //// Part one: New FID => Old FID
    if ($data_type == 'src_fid') {
      $query = Database::getConnection('default', 'default')
        ->select('migrate_map_migratewebformfilemanagedeoss', 'mfm')
        ->fields('mfm', array('sourceid1'))
        ->condition('mfm.destid1', $fid);
      $results = $query->execute()->fetchAll();
      if (count($results) == 1) {
        //// Part two: Old FID => All Old Fid data
        $query2 = Database::getConnection('default', 'eossMigrate')
          ->select('file_managed', 'fm')
          ->fields('fm')
          ->condition('fm.fid', $results[0]->sourceid1);
        $results = $query2->execute()->fetchAll();
      }
    }

    // Process the results
    if (count($results) <> 1) {
      Migration::displayMessage(__METHOD__ . ' - Number of records for source file ID ' . $fid . ' is not SOMTHING like 1.');
      return FALSE;
    }
    else {
      foreach ($results as $result) {
        return $result;
      }
    }
  }

  /**
   * Get portion of file location in OS, based on URI
   *
   * @param $uri - URI of file_managed DB entry
   * @param (bool) $is_uri_base - If TRUE, always return publicFolder
   *   (private vs. public managed in URI stream wrapper)
   * @return (string) URI-determined portion of file location.
   */
  public static function isUriPrivate($uri, $for_uri = FALSE) {
    if (!$for_uri) { // for inclusion in file path
      return (preg_match('|^private://(.+)$|', $uri)) ? self::$privateFolder : self::$publicFolder;
    }
    else { // for inclusion in the URI
      return self::$publicFolder;
    }
  }
}
