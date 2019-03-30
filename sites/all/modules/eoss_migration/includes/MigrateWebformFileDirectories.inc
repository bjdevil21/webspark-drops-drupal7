<?php

/**
 * Creates directory information for file moves/migrations
 */
class MigrateWebformFileDirectories {

  /**
   * Directories for file migration (src, dest)
   * @var
   */
  //@TODO -- Get new folder name into these
  protected $srcFolder = '/home/bryan/eoss-migrate/sites/default/files/webform/';
  protected $dstFolder = DRUPAL_ROOT . '/sites/default/files/webform/';

  public function getSrcFolder() {
    return $this->srcFolder;
  }
  public function getDstFolder() {
    return $this->dstFolder;
  }

  /**
   * @var (obj upon instantiation) List of directory names
   */
  protected $directoryNames;
  public function getDirectoryNames() {
    return $this->directoryNames;
  }
  public function setDirectoryNames($directoryNames) {
    $this->directoryNames = $directoryNames;
  }

  /**
   * MigrateWebformFileDirectories constructor.
   *
   * Builds default directory names list upon each instantiation.
   */
  public function __construct($node_id = 0) {

    // INIT directoryNames variable
    $this->setDirectoryNames(new stdClass());

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
    $query->orderBy('n.nid');
    $results = $query->execute()->fetchAll();

    foreach ($results as $result) {
      $title = substr($result->title, 0, 20);
      $this->directoryNames->{$result->uuid} = $result->nid . '__' . urlencode(str_replace(' ', '-', $title));
    }
    if (count($results) === 1) { // set special property for single output
      $this->singleDir = $this->directoryNames->{$result->uuid};
    }
  }

  /**
   * @return string - The directory given the number of node IDs == 1
   */
  public function __toString() {
    return (string) $this->singleDir;
  }

  /**
   * Return information about file in file_managed.fid for better directory structure
   * for webform submitted files.
   *
   * @param string $fid - file ID from either file_managed or file_usage DB tables
   * @param string $db_conn - should be default
   * @return bool if FALSE, else object $result
   */
  public static function getInfoFromFid( string $fid, $db_conn = 'default') {
    // @TODO -- 3) prepareRow will update each fm.uri, and
    $migration = Migration::currentMigration();
    $query = Database::getConnection('default', $db_conn)
      ->select('file_managed', 'fm');
    $query->innerJoin('file_usage', 'fu', 'fm.fid = fu.fid');
    $query->innerJoin('webform_submissions', 'ws', 'fu.id = ws.sid');
    $query->innerJoin('node', 'n', 'ws.nid = n.nid');
    $query->fields('fm', array('fid', 'uri'))
      ->fields('n', array('machine_name'))
      ->isNotNull('ws.nid')
      ->condition('fm.uri', 'p%', 'LIKE')
      ->condition('fu.module', 'webform', 'LIKE')
      ->condition('fm.fid', $fid)
      ->orderBy('ws.nid');
    $result = $query->execute()->fetchObject();
    if (!$result) {
      Migration::displayMessage(__METHOD__ . ' - Number of records for file ID ' . $fid . ' is not exactly 1.');
      return FALSE;
    }
    else {
      return $result;
    }
  }

}
