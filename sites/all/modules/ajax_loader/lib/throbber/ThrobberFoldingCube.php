<?php

/**
 * Class ThrobberFoldingCube
 */
class ThrobberFoldingCube extends ThrobberBase {

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber sk-folding-cube">
              <div class="sk-cube1 sk-cube"></div>
              <div class="sk-cube2 sk-cube"></div>
              <div class="sk-cube4 sk-cube"></div>
              <div class="sk-cube3 sk-cube"></div>
            </div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/folding-cube.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Folding cube';
  }

}