<?php
/**
 * @file
 * Code for the Lndr module
 */



/**
 * Form builder callback
 */
function lndr_admin_settings($form, &$form_state) {

  // @todo: we probably also want to provide a way for user to test their settings
  $form['lndr_token'] = array(
    '#type' => 'textfield',
    '#title' => t('API token'),
    '#default_value' => variable_get('lndr_token', ''),
    '#description'   => t("Your Lndr account API token, you can find this information under your user profile in Lndr"),
  );

  return system_settings_form($form);
}

/**
 * Form validation handler
 */
function lndr_admin_settings_validate($form, &$form_state) {
  // It be better if Lndr can validate the API token
}