<?php

/**
 * Class ThrobberThreeBounce
 */
class ThrobberThreeBounce extends ThrobberBase {

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber sk-three-bounce">
              <div class="sk-child sk-bounce1"></div>
              <div class="sk-child sk-bounce2"></div>
              <div class="sk-child sk-bounce3"></div>
            </div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/three-bounce.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Three bounce';
  }

}