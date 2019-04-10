<?php

/**
 * Class ThrobberWanderingCubes
 */
class ThrobberWanderingCubes extends ThrobberBase {

  /**
   * @inheritdoc
   */
  protected function setMarkup() {
    return '<div class="ajax-throbber sk-wandering-cubes">
              <div class="sk-cube sk-cube1"></div>
              <div class="sk-cube sk-cube2"></div>
            </div>';
  }

  /**
   * @inheritdoc
   */
  protected function setCssFile() {
    return $this->path . '/css/wandering-cubes.css';
  }

  /**
   * @inheritdoc
   */
  protected function setLabel() {
    return 'Wandering cubes';
  }

}