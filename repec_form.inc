<?php

/**
 * @file
 * This module implements the form for RePEc settings.
 */

/**
 * Settings Form at admin/config/services/repec.
 */
function repec_settings_form($form, &$form_state) {

  $form['description_markup'] = array(
    '#type' => 'markup',
    '#markup' => t('This module helps you create a repository for') . ' <a href="http://repec.org" target="_blank">' . t('Research Papers in Economics') . '</a> ' . t('(RePEc). A template is created an updated automatically when new content is added to a content type matched in the Paper Series. <br/>
RePEc is a collaborative effort of hundreds of volunteers in 82 countries to enhance the dissemination of research in Economics and related sciences. Collected papers are searchable from the ') . '<a href="http://ideas.repec.org" target="_blank">' . t('RePEc Ideas website.') . '</a>',
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  // Archive code provided by RePEc.
  $form['repec_archive_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Archive Code'),
    '#description' => t('This code must be registered and provided by RePEc. This has three letters.'),
    '#default_value' => variable_get('repec_archive_code'),
    '#maxlength' => 3,
    '#required' => TRUE,
  );

  // Main directory to store repec templates.
  $form['repec_base_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Base Path'),
    '#description' => t('This is the path for the main directory to store the template files. This is the directory to be checked by RePEc system. You must setup this path on the filesystem.'),
    '#default_value' => variable_get('repec_base_path'),
    '#required' => TRUE,
  );

  // Provider.
  $provider = variable_get('repec_provider');
  $form['repec_provider'] = array(
    '#type' => 'fieldset',
    '#title' => t('Provider'),
    '#tree' => true,
  );

  // Provider Name.
  $form['repec_provider']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Provider Name'),
    '#description' => t('This is the name of the provider institution (example: University of Southern California, Lusk Center for Real Estate).'),
    '#default_value' => !empty($provider['name']) ? $provider['name'] : '',
    '#required' => TRUE,
  );

  // Provider Homepage.
  $form['repec_provider']['homepage'] = array(
    '#type' => 'textfield',
    '#title' => t('Provider Homepage'),
    '#description' => t('This is the homepage of the provider institution (example: http://lusk.usc.edu).'),
    '#default_value' => !empty($provider['homepage']) ? $provider['homepage'] : '',
    '#maxlength' => 60,
    '#required' => TRUE,
  );

  // Provider Institution.
  $form['repec_provider']['institution'] = array(
    '#type' => 'textfield',
    '#title' => t('Provider Institution'),
    '#description' => t('This is the provider institution (example: RePEc:edi:lcuscus).'),
    '#default_value' => !empty($provider['institution']) ? $provider['institution'] : '',
    '#required' => TRUE,
  );

  // Maintainer.
  $maintainer = variable_get('repec_maintainer');
  $form['repec_maintainer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Maintainer'),
    '#tree' => true,
  );

  // Maintainer Name.
  $form['repec_maintainer']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Maintainer Name'),
    '#description' => t('This is the name of the maintainer.'),
    '#default_value' => !empty($maintainer['name']) ? $maintainer['name'] : '',
    '#required' => TRUE,
  );

  // Maintainer Email.
  $form['repec_maintainer']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Maintainer Email'),
    '#description' => t('This is the email of the maintainer.'),
    '#default_value' => !empty($maintainer['email']) ? $maintainer['email'] : '',
    '#required' => TRUE,
  );

  // Provider.
  $series = variable_get('repec_series');
  $form['repec_series'] = array(
    '#type' => 'fieldset',
    '#title' => t('Series'),
    '#tree' => true,
  );

  // Series.
  $form['repec_series']['name'] = array(
    '#type' => 'select',
    '#title' => t('Series'),
    '#options' => array(
      'paper' => t('Paper Series'),
    ),
    '#default_value' => !empty($series['name']) ? $series['name'] : '',
    '#description' => t("Select the type of series (List of") . " <a href='http://ideas.repec.org/t/seritemplate.html' target='_blank'>" . t("available series") . "</a>)",
  );

  // Paper series.
  $form['repec_series']['repec_paper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Paper Series'),
    '#states' => array(
      'visible' => array(
        'select[name="repec_series_name"]' => array('value' => "paper"),
      ),
    ),
    '#tree' => true,
  );

  // Name for paper templates.
  $form['repec_series']['repec_paper']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Paper Series Name'),
    '#description' => t('This is the name for the paper series. (example: Working Paper)'),
    '#default_value' => !empty($series['repec_paper']['name']) ? $series['repec_paper']['name'] : 'Working Paper',
  );

  // Directory to store paper templates.
  $form['repec_series']['repec_paper']['dir'] = array(
    '#type' => 'textfield',
    '#title' => t('Paper Directory'),
    '#description' => t('This is the directory for the paper templates. This has six letters (example: wpaper). You must setup this directory on your filesystem'),
    '#default_value' => !empty($series['repec_paper']['dir']) ? $series['repec_paper']['dir'] : 'wpaper',
    '#maxlength' => 6,
  );

  // Content Type to match.
  $types = node_type_get_types();
  $type_names = array();
  $type_names['none'] = t('Select Content Type');
  foreach ($types as $key => $type) {
    $type_names[$key] = t('@name', array('@name' => $type->name));
  }

  $form['repec_series']['repec_paper']['content_type'] = array(
    '#type' => 'select',
    '#title' => t('Content Types'),
    '#options' => $type_names,
    '#description' => t('Select the content type associated to this series.'),
    '#ajax' => array(
      'callback' => 'repec_ajax_fieldset_association',
      'wrapper' => 'field_association_fieldset',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#default_value' => !empty($series['repec_paper']['content_type']) ? $series['repec_paper']['content_type'] : '',
  );

  $form['repec_series']['repec_paper']['field_association'] = array(
    '#title' => t('Field Association'),
    '#type' => 'fieldset',
    '#description' => t($content_type . 'Please, associate fields from the selected content type to the series template. This will automatically be persisted.'),
    '#prefix' => '<div id="field_association_fieldset">',
    '#suffix' => '</div>',
    '#tree' => true,
    '#states' => array(
      'visible' => array(
        'select[name="repec_series][repec_paper][content_type]"]' => array('!value' => "none"),
      ),
    ),
  );

  // Setting the default values of the variables.

  $form_state['values']['repec_series']['name']                        = empty($form_state['values']['repec_series']['name']) ? $series['name'] : $form_state['values']['repec_series']['name'];
  $form_state['values']['repec_series']['repec_paper']['name']         = empty($form_state['values']['repec_series']['repec_paper']['name']) ? $series['repec_paper']['name'] : $form_state['values']['repec_series']['repec_paper']['name'];
  $form_state['values']['repec_series']['repec_paper']['content_type'] = empty($form_state['values']['repec_series']['repec_paper']['content_type']) ? $series['repec_paper']['content_type'] : $form_state['values']['repec_series']['repec_paper']['content_type'];

  $content_type = $form_state['values']['repec_series']['repec_paper']['content_type'];
  if ($content_type != NULL) {
    repec_fields_association($form, $form_state, $content_type);
  }

  form_load_include($form_state, 'inc', 'repec', 'repec_form');
  return system_settings_form($form);
}
 
 
/**
 * Implements hook_form_alter().
 */
function repec_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'repec_settings_form') {
    // Add specific css for the settings page.
    $form['#attached']['css'][] = drupal_get_path('module', 'repec') . '/css/repec.css';
    array_unshift($form['#submit'], 'repec_settings_form_submit');
    array_unshift($form['#validate'], 'repec_settings_form_validate');
  }
}

/**
 * This function recreates the fieldset_association.
 */
function repec_ajax_fieldset_association($form, &$form_state) {
  return $form['repec_series']['repec_paper']['field_association'];
}

/**
 * This function associates the fields.
 */
/**
 * This is triggered by checked content type radiobutton.
 */
function repec_associate_fields($form_state) {
  if (!empty($form_state['triggering_element']) && $form_state['triggering_element']['#id'] == 'add_button') {
    $content_type = $form_state['values']['repec_series']['repec_paper']['content_type'];
    $series = $form_state['values']['repec_series']['name'];

    $field_content_type = $form_state['values']['repec_series']['repec_paper']['field_association']['new_field_ct'];
    $field_series = $form_state['values']['repec_series']['repec_paper']['field_association']['new_field_series'];

    if ($field_content_type != 'none' && $field_series != 'none') {
      module_load_include('inc', 'repec', 'repec_aux');
      repec_add_association($content_type, $series, $field_content_type, $field_series);
    }
  }
}

/**
 * This function disassociates the fields.
 */
function repec_disassociate_fields($form_state) {
  // This is triggered by unchecked field series
  // checkbox and content type select box.
  if (!empty($form_state['triggering_element']) && strpos($form_state['triggering_element']['#id'], 'remove_button') !== FALSE) {
    $content_type = $form_state['values']['repec_series']['repec_paper']['content_type'];
    $series = $form_state['values']['repec_series']['name'];

    $field_series = $form_state['triggering_element']['#name'];
    module_load_include('inc', 'repec', 'repec_aux');
    repec_delete_association($content_type, $series, NULL, $field_series);
  }
}

/**
 * Displays the association for template - content_type fields.
 */
function repec_fields_association(&$form, &$form_state, $content_type) {

  repec_disassociate_fields($form_state);
  repec_associate_fields($form_state);

  module_load_include('inc', 'repec', 'repec_aux');
  $ct_fields = field_info_instances('node', $content_type);

  $form['repec_series']['repec_paper']['field_association']['#title'] = t("Fields for !content", array('!content' => node_type_get_name($content_type)));

  // Fields for the Series Template.
  $series = $form_state['values']['repec_series'];
  $series_fields = array();

  $mapping = module_invoke_all('repec_paper_mapping', NULL);
  $node = NULL;
  drupal_alter('repec_paper_mapping', $mapping, $node);

  foreach ($mapping as $item) {
    if (isset($item['visible']) && $item['visible']) {
      $series_fields[drupal_clean_css_identifier($item['property'])] = $item['property'];
    }
  }

  $available_fields_series = array();
  $available_fields_ct = array();

  foreach ($ct_fields as $field_name => $value) {
    $available_fields_ct[$field_name] = $value['label'];
  }

  $count_series = 0;
  foreach ($series_fields as $field_name => $value) {
    $field_content_type = repec_get_field_content_type($content_type, $series['name'], $field_name);
    if ($field_content_type == FALSE) {
      $available_fields_series[$field_name] = t('!value', array('!value' => $value));
    }
    else {
      unset($available_fields_ct[$field_content_type]);
    }

    if ($field_content_type != FALSE) {
      $form['repec_series']['repec_paper']['field_association']['field_series_' . $count_series] = array(
        '#type' => 'select',
        '#options' => array(
          'none' => t('Select a series field'),
          $field_name => t('!value', array('!value' => $value)),
        ),
        '#value' => $field_name,
        '#prefix' => "<div class='select_series_fields'>",
        '#suffix' => "</div>",
        '#disabled' => TRUE,
      );

      $form['repec_series']['repec_paper']['field_association']['field_ct_' . $count_series] = array(
        '#type' => 'select',
        '#options' => array(
          'none' => t('Select a content type field'),
          $field_content_type => t('!value',
          array('!value' => $ct_fields[$field_content_type]['label'])),
        ),
        '#value' => $field_content_type,
        '#prefix' => "<div class='select_content_type'>",
        '#suffix' => "</div>",
        '#disabled' => TRUE,
      );
      $form['repec_series']['repec_paper']['field_association']['remove_' . $count_series++] = array(
        '#id' => 'remove_button_' . $field_name,
        '#name' => $field_name,
        '#type' => 'button',
        '#value' => t('Remove'),
        '#ajax' => array(
          'callback' => 'repec_ajax_fieldset_association',
          'wrapper' => 'field_association_fieldset',
          'prevent' => 'submit',
        ),
        '#prefix' => "<div class='button_remove'>",
        '#suffix' => "</div>",
      );

      if ($field_name == 'author') {
        $form['repec_series']['repec_paper']['field_association']['repec_check_delimiters'] = array(
          '#type' => 'checkbox',
          '#title' => t('Check to split the field by delimiters ( and & , ; )'),
          '#prefix' => "<div class='check_delimiters'>",
          '#suffix' => "</div>",
          '#default_value' => empty($form_state['values']['repec_check_delimiters']) ? variable_get('repec_check_delimiters', 1) : $form_state['values']['repec_check_delimiters'],
        );
      }
    }
  }

  // Select Box.
  $form['repec_series']['repec_paper']['field_association']['new_item'] = array(
    '#title' => t('Choose a new association'),
    '#type' => 'item',
    '#prefix' => "<div class='new_item'>",
    '#suffix' => "</div>",
  );

  // Select Box.
  $form['repec_series']['repec_paper']['field_association']['new_field_series'] = array(
    '#type' => 'select',
    '#options' => array_merge(array('none' => 'Select a series field'), $available_fields_series),
    '#default_value' => 'none',
    '#prefix' => "<div class='new_field'>",
    '#suffix' => "</div>",
  );

  // Select Box.
  $form['repec_series']['repec_paper']['field_association']['new_field_ct'] = array(
    '#type' => 'select',
    '#options' => array_merge(array('none' => 'Select a content type field'), $available_fields_ct),
    '#default_value' => 'none',
    '#prefix' => "<div class='new_field'>",
    '#suffix' => "</div>",
  );

  $form['repec_series']['repec_paper']['field_association']['add_' . $count_series++] = array(
    '#type' => 'button',
    '#id' => 'add_button',
    '#value' => t('Add'),
    '#ajax' => array(
      'callback' => 'repec_ajax_fieldset_association',
      'wrapper' => 'field_association_fieldset',
      'prevent' => 'submit',
    ),
    '#prefix' => "<div class='button_add'>",
    '#suffix' => "</div>",
  );
}

/**
 * Validate Settings Form.
 */
function repec_settings_form_validate (&$form, &$form_state) {
  $archive_code = $form_state['values']['repec_archive_code'];
  $repec_series = $form_state['values']['repec_series'];

  if (strlen($archive_code) != 3) {
    form_set_error('repec_archive_code', t('Archive code must have three letters.'));
  }
  if (strlen($repec_series['repec_paper']['dir']) != 6) {
    form_set_error('repec_series][repec_paper][dir', t('Paper directory must have 6 letters.'));
  }
}

/**
 * Submit Settings Form.
 */
function repec_settings_form_submit(&$form, &$form_state) {
  $repec_series = variable_get('repec_series');
  module_load_include('inc', 'repec', 'repec_aux');
  repec_enable_association($repec_series['repec_paper']['content_type'], $repec_series['name']);
  repec_create_directory();
}
