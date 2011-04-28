FILENAME : includes/form.api
LINE No. 1870

----------------------------------------------------------------------------------------------------
/**
 * Format a checkbox.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used:  title, value, return_value, description, required
 * @return
 *   A themed HTML string representing the checkbox.
 *
 * @ingroup themeable
 */
function theme_checkbox($element) {
  _form_set_class($element, array('form-checkbox'));
  $checkbox = '<input ';
  $checkbox .= 'type="checkbox" ';
  $checkbox .= 'name="'. $element['#name'] .'" ';
  $checkbox .= 'id="'. $element['#id'] .'" ' ;
  $checkbox .= 'class="'. $element['#options_class'] . '" '; /************************* CUSTOM CODE BY PRASHANT SHAH *************************/
  $checkbox .= 'value="'. $element['#return_value'] .'" ';
  $checkbox .= $element['#value'] ? ' checked="checked" ' : ' ';
  $checkbox .= drupal_attributes($element['#attributes']) .' />';

  if (!is_null($element['#title'])) {
    $checkbox = '<label class="option ' . $element['#options_class'] . '" for="'. $element['#id'] .'">'. $checkbox .' '. $element['#title'] .'</label>';  /************************* CUSTOM CODE BY PRASHANT SHAH *************************/
  }

  unset($element['#title']);
  return theme('form_element', $element, $checkbox);
}

/**
 * Format a set of checkboxes.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string representing the checkbox set.
 *
 * @ingroup themeable
 */
function theme_checkboxes($element) {
  $class = 'form-checkboxes';
  if (isset($element['#attributes']['class'])) {
    $class .= ' '. $element['#attributes']['class'];
  }
  $element['#children'] = '<div class="'. $class .'">'. (!empty($element['#children']) ? $element['#children'] : '') .'</div>';
  if ($element['#title'] || $element['#description']) {
    unset($element['#id']);
    return theme('form_element', $element, $element['#children']);
  }
  else {
    return $element['#children'];
  }
}

function expand_checkboxes($element) {
  $value = is_array($element['#value']) ? $element['#value'] : array();
  $element['#tree'] = TRUE;
  if (count($element['#options']) > 0) {
    if (!isset($element['#default_value']) || $element['#default_value'] == 0) {
      $element['#default_value'] = array();
    }
    foreach ($element['#options'] as $key => $choice) {
      if (!isset($element[$key])) {
        $element[$key] = array(
          '#type' => 'checkbox',
          '#processed' => TRUE,
          '#title' => $choice,
          '#return_value' => $key,
          '#default_value' => isset($value[$key]),
          '#attributes' => $element['#attributes'],
          '#ahah' => isset($element['#ahah']) ? $element['#ahah'] : NULL,
          '#options_class' => $element['#options_class'][$key], /************************* CUSTOM CODE BY PRASHANT SHAH *************************/
        );
      }
    }
  }
  return $element;
}

