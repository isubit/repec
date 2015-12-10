<?php

/**
 * @file
 * This module implements the integration between Content Type and RePEc.
 */

/**
 * This function creates directory based on the settings informed by the user.
 */
function repec_create_directory() {
  $repec_base_path = variable_get('repec_base_path');
  $repec_archive_code = variable_get('repec_archive_code');
  $repec_series = variable_get('repec_series');
  $repec_papers_path = $repec_base_path . '/' . $repec_archive_code . '/' . $repec_series['repec_paper']['dir'] . '/';

  if (!file_exists($repec_base_path)) {
    if (!mkdir($repec_base_path, 0755, TRUE)) {
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

  // This function will generate the directory
  // and the archive and series templates.
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
 * This function generates the repec template for the article.
 */
function repec_generate_paper_template($node) {
  $repec_base_path = variable_get('repec_base_path');
  $repec_archive_code = variable_get('repec_archive_code');
  $repec_series = variable_get('repec_series');

  // Based on this url: http://ideas.repec.org/t/papertemplate.html
  // This function returns the path for the template generated.
  $repec_papers_path = $repec_base_path . '/' . $repec_archive_code . '/' . $repec_series['repec_paper']['dir'] . '/';
  $content_type = $repec_series['repec_paper']['content_type'];

  if ($node->type == $content_type) {
    $file_name = $repec_series['repec_paper']['dir'] . '_' . $node->vid . '.rdf';

    if (!file_exists($repec_papers_path)) {
      if (!mkdir($repec_papers_path, 0755, TRUE)) {
        drupal_set_message(t("Directory couldn't be created in this path"), 'error');
        return;
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
      return;
    }

    return $repec_papers_path . $file_name;
  }
}

/**
 * Remove a template file.
 */
function repec_remove_paper_template($node) {
  $repec_base_path = variable_get('repec_base_path');
  $repec_archive_code = variable_get('repec_archive_code');
  $repec_series = variable_get('repec_series');

  $repec_papers_path = $repec_base_path . '/' . $repec_archive_code . '/' . $repec_series['repec_paper']['dir'] . '/';
  $file_name = $repec_series['repec_paper']['dir'] . '_' . $node->vid . '.rdf';
  unlink($repec_papers_path . $file_name);
}

/**
 * This function generates the repec template for the series.
 */
function repec_generate_series_template() {
  $repec_base_path = variable_get('repec_base_path');
  $repec_archive_code = variable_get('repec_archive_code');

  // Based on this url: http://ideas.repec.org/t/seritemplate.html.
  // This function returns the path for the template generated.
  $repec_archive_path = $repec_base_path . '/' . $repec_archive_code . '/';
  if (!file_exists($repec_archive_path)) {
    if (!mkdir($repec_archive_path, 0777, TRUE)) {
      drupal_set_message(t("Directory couldn't be created in this path"), 'error');
    }
  }

  $file_name = $repec_archive_code . 'seri.rdf';

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
 * This function generates the repec template for the series.
 */
function repec_generate_archive_template() {
  $repec_base_path = variable_get('repec_base_path');
  $repec_archive_code = variable_get('repec_archive_code');

  // Based on this url: http://ideas.repec.org/t/archtemplate.html.
  // This function returns the path for the template generated.
  $repec_archive_path = $repec_base_path . '/' . $repec_archive_code . '/';
  if (!file_exists($repec_archive_path)) {
    if (!mkdir($repec_archive_path, 0777, TRUE)) {
      drupal_set_message(t("Directory couldn't be created in this path"), 'error');
    }
  }

  $file_name = $repec_archive_code . 'arch.rdf';

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
function repec_generate_array_authors($authors_names) {

  // Delimiters (and, &, ',', ';', with).
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

  if ($row) {
    return $row->field_content_type;
  }
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
