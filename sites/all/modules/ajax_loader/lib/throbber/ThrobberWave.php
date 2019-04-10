<?php

/**
 * Class ThrobberWave
 */
class ThrobberWave extends ThrobberBase {

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber sk-wave">
              <div class="sk-rect sk-rect1"></div>
              <div class="sk-rect sk-rect2"></div>
              <div class="sk-rect sk-rect3"></div>
              <div class="sk-rect sk-rect4"></div>
              <div class="sk-rect sk-rect5"></div>
            </div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/wave.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Wave';
  }

}