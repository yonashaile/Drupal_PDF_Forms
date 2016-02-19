<section id="PDFFormSigPadFields" ng-app="pdfSigPadFields" ng-controller="MainCtrl"
  data-type="<?php print $type; ?>"
  data-page="<?php print $page; ?>">
    <div class="left">
      <div class="canvas">
        
        <div class="inner">
          <sig-pad-fields>
            <sig-field ng-repeat="field in fields track by $index" ng-click="editSigField($index)" data-index="{{$index}}"
            style="top:{{field.top}}px; left:{{field.left}}px; z-index:{{$index}}; width:{{field.width}}px; height:{{field.height}}px;">
              {{field.label}}
            </sig-field>
          </sig-pad-fields>
          <sig-pad-page data-image="<?php print $image; ?>">
            <img src="<?php print $image; ?>" width="100%" height="100%">
          </sig-pad-page>
        </div>
      </div>
    </div>
    <div class="right">
      <button id="addField" class="btn btn-info" ng-click="addSigField(null)">Add Signature Field</button>
      <div class="signatureControls">

          <fieldset id="controllPaletteForm">
            <legend>Field Settings</legend>
            <div class="form-item">
              <label>Field Name</label><input type="textfield" class="form-text" id="field_name" ng-model="activeField.label">
            </div>
            <div class="form-item">
              <label>Field Value</label><input type="textfield" class="form-text" id="field_value" ng-model="activeField.value">
            </div>

          </fieldset>

          <div id="actions">
            <button class="btn btn-info" ng-click="saveSettings();">Save Settings</button>
            <button class="btn btn-warning" ng-click="cancelChanges();">Cancel</button>
          </div>

      </div>

      <ul class="fieldList">
        <li ng-repeat="field in fields track by $index" ng-click="editSigField($index)" data-index="{{$index}}">{{field.label}}</li>
      </ul>
    </div>
</section>
