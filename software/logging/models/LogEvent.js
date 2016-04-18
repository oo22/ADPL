/*
 * The schema to hold incoming events
 */
var mongoose = require('mongoose');
var Schema = mongoose.Schema;
var LogEvent  = new Schema({ 
	coreid:		{type: String, required: true, trim: true},  // coreid of sender
	loc:		{type: String, required: true, trim: true}, // Location identifier 
	time:		{type: Date, required: true}, // Time the data was published
	probeid:	{type: Number, required: true}, // the pin the probe is on
	temp:		{type: Number, required: true} // in degrees C
});
module.exports = mongoose.model('LogEvent', LogEvent);