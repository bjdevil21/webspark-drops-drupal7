<?php

/**
 * Confirmation page callback for COM form.
 *
 * @return array
 */
function asu_comrf_confirmation_page() {
  // Define markup for confirmation page and return the markup
  $markup = variable_get('asu_comrf_form_confirmation_text_markup', '<div>There was an issue loading the Change of Major confirmation page content.</div>');
  return array(
    '#markup' => $markup,
  );
}

/**
 * Error page callback for COM form.
 *
 * @return array
 */
function asu_comrf_notice_page() {
  // Define markup for error page and return the markup
  $markup = variable_get('asu_comrf_form_notice_text_markup', '<div>There was an issue loading the Change of Major notice page content.</div>');
  return array(
    '#markup' => $markup,
  );
}