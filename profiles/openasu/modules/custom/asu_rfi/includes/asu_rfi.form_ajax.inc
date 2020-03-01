<?php

/**
 * Change of Major request form AJAX callback used to update specific AJAX fields listed below.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function asu_rfi_campus_ajax_callback($form, &$form_state) {
  dpm(__FUNCTION__);
  $commands = array(
    ajax_command_html("#edit-my-interest .form-item-prog-plan label", "<h2>_html - My interest" . time() . "</h2>"),
    ajax_command_replace("#asu-rfi-start-date-ajax-wrapper", render($form['my_interests']['start_date'])),
    ajax_command_html("#edit-about-me .fieldset-legend", "<h2>_html - ABOUT ME - legend" . time() . "</h2>"),
  );
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}
