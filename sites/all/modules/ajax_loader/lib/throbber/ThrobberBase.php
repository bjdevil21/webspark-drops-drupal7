<?php

/**
 * Class ThrobberBase
 */
abstract class ThrobberBase implements ThrobberInterface {

  protected $path;
  protected $markup;
  protected $css_file;
  protected $label;


  /**
   * ThrobberBase constructor.
   */
  public function __construct() {
    $this->path = drupal_get_path('module', 'ajax_loader');
    $this->markup = $this->setMarkup();
    $this->css_file = $this->setCssFile();
    $this->label = $this->setLabel();
  }

  /**
   * @return mixed
   */
  public function getMarkup() {
    return $this->markup;
  }

  /**
   * @return mixed
   */
  public function getCssFile() {
    return $this->css_file;
  }

  /**
   * @return mixed
   */
  public function getLabel() {
    return $this->label;
  }

  /**
   * Sets markup for throbber.
   */
  protected abstract function setMarkup();

  /**
   * Sets css file for throbber.
   */
  protected abstract function setCssFile();

  /**
   * Sets label for throbber.
   */
  protected abstract function setLabel();

}