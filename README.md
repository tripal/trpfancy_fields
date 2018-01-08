# Tripal Fancy Fields

**UNDER ACTIVE DEVELOPMENT**

**NOT READY FOR USE**

## How To Use this Module
### Through the UI
If you simply want to use the existing charts, you can add these fields through the "Manage Fields" user interface. There are a large number of customization options available including support for all TripalD3 chart options and the ability to set the materialized view, columns and a filter field to generate the data based on.
1. Go to the "Manage Fields" interfance for the Tripal Content Type you would like to add the Tripal Fancy Field to. This can be found at Administration Toolbar > Structure > Tripal Content Types > `Type of your choice` > Manage Fields.
2. Scroll to the bottom of the form where it says "Add new field". Set a descriptive label for your chart, select the type of Tripal Fancy field you would like to add from the "Select a field type" dropdown. Then click the "Save" button at the bottom of the page.
3. Fill out the settings for this field for the content type you chose (automatically shown when you complete step 3). **Especially make sure to set a descriptive controlled vocabulary term at this step!** The controlled vocabulary term will be how users access this data through web-services and how it will be available to other Tripal Sites.
6. Finally, make your new field show up on the page by navigating to "Manage Display" and dragging it where you would like it. By default it will be in the "Disabled" section at the bottom of the page. If you would like it in it's own Tripal Pane, create one using the "Create empty Tripal Pane" link at the top of the page.

### Extend the Base Classes
If you would like even more control over the field then is provided through the UI, you can extend the ChadoChart base classes in your own custom module.
1. Create a custom module. See the [Drupal Documentation](https://www.drupal.org/docs/7/creating-custom-modules) for how to do this.
2. Create your child class extending the base class of your choice in `[your module]/includes/TripalFields/[field name]/[field name].inc`
3. Set the various settings under `EDITABLE STATIC CONSTANTS`.
4. Implement hook_bundle_fields_info() and hook_bundle_instances_info() to attach your field to a Tripal Content Type or add it through the UI just like you would a regular Tripal Fancy Field.

## Single-Series Chart Field
**UNDER DEVELOPMENT**

This field is designed to make adding single-series charts to your Tripal Content Pages easy! The data is pulled from a materialized view of your choosing and filtered based on a specified field. For example, you could choose to add a chart to your organism pages showing the proportion of feature types for that organism. There are multiple display types available for this field including: pie, donut and bar charts, as well as, a simple table.
