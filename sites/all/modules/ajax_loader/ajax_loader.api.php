<?php

/**
 * This hook allows you to add extra throbbers.
 *
 * @return array
 *  An array of throbbers.
 *  Each throbber must contain the key "class" which refers to the class name of the throbber.
 *  Make sure your class implements "ThrobberInterface" or extends "ThrobberBase".
 */
function hook_ajax_loader() {
  $throbbers['new_throbber'] = array(
    'class' => 'MyNewThrobber',
  );
  $throbbers['another_throbber'] = array(
    'class' => 'AnotherThrobber',
  );
  return $throbbers;
}
