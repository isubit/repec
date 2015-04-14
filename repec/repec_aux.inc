<?php

/**
 * @file
 * This module implements the integration between Content Type and RePEc.
 */

/**
 * This function creates directory based on the settings informed by the user.
 */
function repec_create_directory() {
  $repec_papers_path = variable_get('repec_base_path') . '/' . variable_get('repec_archive_code') . '/' . variable_get('repec_paper_dir', 'wpaper') . '/';

  if (!file_exists(variable_get('repec_base_path'))) {
    if (!mkdir(variable_get('repec_base_path'), 0755, TRUE)) {
      drupal_set_message(t("Directory couldn't be created in this path"), 'error');
    }
  }

  // Remove all files of type .rdf
  $files = glob($repec_papers_path . '/*.rdf');
  foreach ($files as $file) {
    if (is_file($file)) {
      unlink($file);
    }
  }

  // This function will generate the directory and the archive and series templates.
  repec_generate_archive_template();
  repec_generate_series_template();

  // Get all contents of type Working Paper.
  $type = "working_paper";
  $nodes = node_load_multiple(array(), array('type' => $type));

  // Save all papers again so that the files can be generated.
  foreach ($nodes as $paper) {
    repec_generate_paper_template($paper);
  }
}

/**
 * Implements hook_repec_paper_mapping().
 */
// It maps the series fields with the node fields to generate the template file.
// The fields displayed on the settings page come from this function.
// To display the fields on the settings page, the field has to have the property 'visible' set to TRUE.
function repec_repec_paper_mapping($node) {

  $mapping = array();

  $node_vars = get_object_vars($node);
  $content_type = variable_get('repec_paper_content_type');
  $series_name = variable_get('repec_series_name');

  $mapping[] = array(
    'property' => 'Template-Type',
    'value' => 'ReDIF-Paper 1.0',
  );

  // Title of the Article.
  $mapping[] = array(
    'property' => 'Title',
    'value' => isset($node->title) ? $node->title : '',
  );

  $author_field = repec_get_field_content_type($content_type, $series_name, 'Author-Name');
  if (!empty($author_field) && !empty($node_vars[$author_field][LANGUAGE_NONE][0]['value'])) {
    $authors = repec_generate_array_authors($node_vars[$author_field][LANGUAGE_NONE][0]['value']);

    foreach ($authors as $author) {
      $mapping[] = array(
        'property' => 'Author-Name',
        'value' => $author,
        'visible' => TRUE,
      );
    }
  }
  else {
    $mapping[] = array(
      'property' => 'Author-Name',
      'value' => '',
      'visible' => TRUE,
    );
  }

  // Abstract of the article.
  $abstract = repec_get_field_content_type($content_type, $series_name, 'Abstract');
  if (!empty($abstract) && !empty($node_vars[$abstract][LANGUAGE_NONE][0]['value'])) {
    $mapping[] = array(
      'property' => 'Abstract',
      'value' => str_replace(array("\r", "\n", "<br />", "<br/>"), "", $node_vars[$abstract][LANGUAGE_NONE][0]['value']),
      'visible' => TRUE,
    );
  }
  else {
    $mapping[] = array(
      'property' => 'Abstract',
      'value' => '',
      'visible' => TRUE,
    );
  }

  // Date of the article.
  $create_date = repec_get_field_content_type($content_type, $series_name, 'Creation-Date');
  if (!empty($create_date) && !empty($node_vars[$create_date][LANGUAGE_NONE][0]['value'])) {
    $mapping[] = array(
      'property' => 'Creation-Date',
      'value' => $node_vars[$create_date][LANGUAGE_NONE][0]['value'],
      'visible' => TRUE,
    );
  }
  else {
    $mapping[] = array(
      'property' => 'Creation-Date',
      'value' => '',
      'visible' => TRUE,
    );
  }

  // File for the article.
  $file_url = repec_get_field_content_type($content_type, $series_name, 'File-URL');
  if (!empty($file_url) && !empty($node_vars[$file_url][LANGUAGE_NONE][0]['fid'])) {
    $file_article = file_load($node_vars[$file_url][LANGUAGE_NONE][0]['fid']);

    $mapping[] = array(
      'property' => 'File-URL',
      'value' => str_replace(" ", "%20", file_create_url($file_article->uri)),
      'visible' => TRUE,
    );
    $mapping[] = array(
      'property' => 'File-Format',
      'value' => ucfirst($file_article->filemime),
    );
  }
  else {
    $mapping[] = array(
      'property' => 'File-URL',
      'value' => '',
      'visible' => TRUE,
    );
  }

  // Keywords.
  $key = repec_get_field_content_type($content_type, $series_name, 'Keywords');
  if (!empty($key) && !empty($node_vars[$key][LANGUAGE_NONE])) {
    $keywords = '';
    foreach ($node_vars[$key][LANGUAGE_NONE] as $keyword) {
      $term = taxonomy_term_load($keyword['tid']);
      $keywords .= $term->name . ', ';
    }
    if (strlen($keywords) > 0) {
      $keywords = substr($keywords, 0, strlen($keywords) - 2);
    }

    $mapping[] = array(
      'property' => 'Keywords',
      'value' => $keywords,
      'visible' => TRUE,
    );
  }
  else {
    $mapping[] = array(
      'property' => 'Keywords',
      'value' => '',
      'visible' => TRUE,
    );
  }

  // Number or id of the Article.
  $mapping[] = array(
    'property' => 'Number',
    'value' => isset($node->nid) ? $node->nid : '',
  );

  // Handle that uniquely identifies the paper.
  $mapping[] = array(
    'property' => 'Handle',
    'value' => 'RePEc:' . variable_get('repec_archive_code') . ':wpaper:' . $node->nid,
  );

  return $mapping;
}

// This function generates the repec template for the article.
// Based on this url: http://ideas.repec.org/t/papertemplate.html
// This function returns the path for the template generated.
function repec_generate_paper_template($node) {
  $repec_papers_path = variable_get('repec_base_path') . '/' . variable_get('repec_archive_code') . '/' . variable_get('repec_paper_dir', 'wpaper') . '/';
  $content_type = variable_get('repec_paper_content_type');
  $series_name = variable_get('repec_series_name');

  if ($node->type == $content_type) {
    $file_name = variable_get('repec_paper_dir', 'wpaper') . '_' . $node->vid . '.rdf';

    if (!file_exists($repec_papers_path)) {
      if (!mkdir($repec_papers_path, 0755, TRUE)) {
        drupal_set_message(t("Directory couldn't be created in this path"), 'error');
      }
    }

    $mapping = module_invoke_all('repec_paper_mapping', $node);
    drupal_alter('repec_paper_mapping', $mapping, $node);

    $content = '';
    foreach ($mapping as $item) {
      if (!empty($item['value'])) {
        $content .= $item['property'] . ': ' . $item['value'] . "\n";
      }
    }

    if (!file_put_contents($repec_papers_path . $file_name, $content)) {
      drupal_set_message(t("File couldn't be created"), "error");
    }

    return $repec_papers_path . $file_name;
  }
}

/**
 * Remove a template file.
 */
function repec_remove_paper_template($node) {
  $repec_papers_path = variable_get('repec_base_path') . '/' . variable_get('repec_archive_code') . '/' . variable_get('repec_paper_dir', 'wpaper') . '/';
  $file_name = variable_get('repec_paper_dir', 'wpaper') . '_' . $node->vid . '.rdf';
  unlink($repec_papers_path . $file_name);
}

/**
 * Implements hook_repec_series_mapping().
 */
// It maps the series attributes to the settings informed by the user to generate the series template file.
// This runs when user saves the settings page.
function repec_repec_series_mapping() {

  $attributes = array(
    array(
      'attribute' => 'Template-type',
      'value' => 'ReDIF-Series 1.0',
    ),
    array(
      'attribute' => 'Name',
      'value' => variable_get('repec_paper_name'),
    ),
    array(
      'attribute' => 'Provider-Name',
      'value' => variable_get('repec_provider_name'),
    ),
    array(
      'attribute' => 'Provider-Homepage',
      'value' => variable_get('repec_provider_homepage'),
    ),
    array(
      'attribute' => 'Provider-Institution',
      'value' => variable_get('repec_provider_institution'),
    ),
    array(
      'attribute' => 'Maintainer-Name',
      'value' => variable_get('repec_maintainer_name'),
    ),
    array(
      'attribute' => 'Maintainer-Email',
      'value' => variable_get('repec_maintainer_email'),
    ),
    array(
      'attribute' => 'Type',
      'value' => 'ReDIF-Paper',
    ),
    array(
      'attribute' => 'Handle',
      'value' => 'RePEc:' . variable_get('repec_archive_code') . ':wpaper',
    ),
  );

  return $attributes;
}

// This function generates the repec template for the series.
// Based on this url: http://ideas.repec.org/t/seritemplate.html.
// This function returns the path for the template generated.
function repec_generate_series_template() {
  $repec_archive_path = variable_get('repec_base_path') . '/' . variable_get('repec_archive_code') . '/';
  if (!file_exists($repec_archive_path)) {
    if (!mkdir($repec_archive_path, 0777, TRUE)) {
      drupal_set_message(t("Directory couldn't be created in this path"), 'error');
    }
  }

  $file_name = variable_get('repec_archive_code') . 'seri.rdf';

  $attributes = module_invoke_all('repec_series_mapping');
  drupal_alter('repec_series_mapping', $attributes);

  $content = '';
  foreach ($attributes as $item) {
    $content .= $item['attribute'] . ': ' . $item['value'] . "\n";
  }

  if (!file_put_contents($repec_archive_path . $file_name, $content)) {
    drupal_set_message(t("File couldn't be created"), "error");
  }
  return $repec_archive_path . $file_name;
}

/**
 * Implements hook_repec_archive_mapping().
 */
// It maps the archive fields with the fields informed by the user on the settings page.
function repec_repec_archive_mapping() {

  $attributes = array(
    array(
      'attribute' => 'Template-type',
      'value' => 'ReDIF-Archive 1.0',
    ),
    array(
      'attribute' => 'Handle',
      'value' => 'RePEc:' . variable_get('repec_archive_code'),
    ),
    array(
      'attribute' => 'Name',
      'value' => variable_get('repec_provider_name'),
    ),
    array(
      'attribute' => 'Maintainer-Name',
      'value' => variable_get('repec_maintainer_name'),
    ),
    array(
      'attribute' => 'Maintainer-Email',
      'value' => variable_get('repec_maintainer_email'),
    ),
    array(
      'attribute' => 'Description',
      'value' => 'This archive collects ' . variable_get('repec_paper_name') . ' from ' . variable_get('repec_provider_name'),
    ),
    array(
      'attribute' => 'URL',
      'value' => variable_get('repec_provider_homepage') . variable_get('repec_base_path') . '/' . variable_get('repec_archive_code') . '/',
    ),
  );

  return $attributes;
}

/**
 * This function generates the repec template for the series.
 */
// Based on this url: http://ideas.repec.org/t/archtemplate.html.
// This function returns the path for the template generated.
function repec_generate_archive_template() {
  $repec_archive_path = variable_get('repec_base_path') . '/' . variable_get('repec_archive_code') . '/';
  if (!file_exists($repec_archive_path)) {
    if (!mkdir($repec_archive_path, 0777, TRUE)) {
      drupal_set_message(t("Directory couldn't be created in this path"), 'error');
    }
  }

  $file_name = variable_get('repec_archive_code') . 'arch.rdf';

  $attributes = module_invoke_all('repec_archive_mapping');
  drupal_alter('repec_archive_mapping', $attributes);

  $content = '';
  foreach ($attributes as $item) {
    $content .= $item['attribute'] . ': ' . $item['value'] . "\n";
  }

  if (!file_put_contents($repec_archive_path . $file_name, $content)) {
    drupal_set_message(t("File couldn't be created"), "error");
  }
  return $repec_archive_path . $file_name;
}

/**
 * This function generates an array of authors.
 */
// Delimiters (and, &, ',', ';', with).
function repec_generate_array_authors($authors_names) {
  // Author Name.
  // Break by and.
  $check_delimiters = variable_get('repec_check_delimiters', 1);
  if (empty($check_delimiters) || $check_delimiters == 0) {
    return array($authors_names);
  }

  $authors_temp = explode(" and ", $authors_names);

  // Break by &.
  $authors = array();
  foreach ($authors_temp as $a) {
    $authors = array_merge($authors, explode("&", $a));
  }

  // Break by ,.
  $authors_temp = $authors;
  $authors = array();
  foreach ($authors_temp as $a) {
    $authors = array_merge($authors, explode(",", $a));
  }

  // Break by ;.
  $authors_temp = $authors;
  $authors = array();
  foreach ($authors_temp as $a) {
    $authors = array_merge($authors, explode(";", $a));
  }

  // Break by with.
  $authors_temp = $authors;
  $authors = array();
  foreach ($authors_temp as $a) {
    $authors = array_merge($authors, explode("with", $a));
  }

  $authors_temp = $authors;
  $authors = array();
  foreach ($authors_temp as $a) {
    $a = trim($a);
    if ($a != NULL && $a != '') {
      $authors[] = $a;
    }
  }
  return $authors;
}

/**
 * This function associates content type and series fields.
 */
function repec_add_association($content_type, $series_name, $field_content_type, $field_series) {
  db_insert('repec_assoc')
    ->fields(array(
      'content_type' => $content_type,
      'series_name' => $series_name,
      'field_content_type' => $field_content_type,
      'field_series' => $field_series,
      'enabled' => 0,
    ))
    ->execute();
}

/**
 * This function disaassociates content type and series fields.
 */
function repec_delete_association($content_type, $series_name, $field_content_type = NULL, $field_series = NULL) {

  $query = db_delete('repec_assoc')
    ->condition('content_type', $content_type)
    ->condition('series_name', $series_name);

  if ($field_content_type != NULL) {
    $query->condition('field_content_type', $field_content_type);
  }
  if ($field_series != NULL) {
    $query->condition('field_series', $field_series);
  }

  $query->execute();
}

/**
 * Returns the associated field_series.
 */
function repec_get_field_series($content_type, $series_name, $field_content_type) {
  $row = db_select('repec_assoc', 'ra')
    ->fields('ra')
    ->condition('content_type', $content_type)
    ->condition('series_name', $series_name)
    ->condition('field_content_type', $field_content_type)
    ->execute()
    ->fetchObject();
  return $row->field_series;
}

/**
 * Returns the associated field of the content type.
 */
function repec_get_field_content_type($content_type, $series_name, $field_series) {
  $row = db_select('repec_assoc', 'ra')
    ->fields('ra')
    ->condition('content_type', $content_type)
    ->condition('series_name', $series_name)
    ->condition('field_series', $field_series)
    ->execute()
    ->fetchObject();
  return $row->field_content_type;
}

/**
 * This enables the association for the informed content type.
 */
function repec_enable_association($content_type, $series_name) {
  db_update('repec_assoc')
    ->fields(array(
      'enabled' => 0,
    ))
    ->condition('series_name', $series_name)
    ->execute();

  db_update('repec_assoc')
    ->fields(array(
      'enabled' => 1,
    ))
    ->condition('content_type', $content_type)
    ->condition('series_name', $series_name)
    ->execute();
}
