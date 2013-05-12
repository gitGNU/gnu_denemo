/******** generated automatically from generate_source. See generate_source.c */
{
"CursorLeft", NULL, N_("Cursor Left"), "", N_("Moves the cursor one object left, altering the selection if any"), G_CALLBACK (cursorleft_cb)},

{
"MoveCursorLeft", NULL, N_("Move Cursor Left"), "", N_("Moves the cursor one object left, without altering the selection"), G_CALLBACK (movecursorleft_cb)},

{
"CursorDown", NULL, N_("Cursor Down"), "", N_("Moves the cursor one scale step down"), G_CALLBACK (cursordown_cb)},

{
"CursorUp", NULL, N_("Cursor Up"), "", N_("Moves the cursor one scale step up"), G_CALLBACK (cursorup_cb)},

{
"CursorRight", NULL, N_("Cursor Right"), "", N_("Moves the cursor one object right, altering the selection if any"), G_CALLBACK (cursorright_cb)},

{
"MoveCursorRight", NULL, N_("Move Cursor Right"), "", N_("Moves the cursor one object right, without altering the selection"), G_CALLBACK (movecursorright_cb)},

{
"GoToMark", NULL, N_("To Mark"), "", N_("Moves the cursor to the Mark without altering the selection"), G_CALLBACK (goto_mark)},

{
"SwapPointAndMark", NULL, N_("Swap Ends of Selection"), "", N_("Swaps the active end of the selection"), G_CALLBACK (swap_point_and_mark)},

{
"GoToSelectionStart", NULL, N_("To Selection Start"), "", N_("Moves the cursor to the first object in the selection without altering the selection. returns #f if no selection"), G_CALLBACK (goto_selection_start)},

{
"PushPosition", NULL, N_("Push Position"), "", N_("Pushes the current cursor position onto a stack"), G_CALLBACK (PushPosition)},

{
"PopPosition", NULL, N_("Pop Position"), "", N_("Pops a position from the stack of cursor positions, moving the cursor there"), G_CALLBACK (PopPosition)},

{
"PopPushPosition", NULL, N_("Pop and Push Position"), "", N_("Pops a position from the stack of cursor positions, pushes the current position, then moves the cursor to the popped position"), G_CALLBACK (PopPushPosition)},

{
"ToggleReduceToDrawingArea", NULL, N_("Hide/Show Menus"), "", N_("Hides/Shows menus, panes etc. The ones shown are those checked in the view menu."), G_CALLBACK (ToggleReduceToDrawingArea)},

{
"StaffUp", NULL, N_("Staff Up"), "", N_("Moves the cursor to the staff above, extending selection if any"), G_CALLBACK (staffup_cb)},

{
"StaffDown", NULL, N_("Staff Down"), "", N_("Moves the cursor to the staff below, extending selection if any"), G_CALLBACK (staffdown_cb)},

{
"MoveToStaffUp", NULL, N_("Move to Staff Up"), "", N_("Moves the cursor to the staff above without altering selection"), G_CALLBACK (movetostaffup_cb)},

{
"MoveToStaffDown", NULL, N_("Move to Staff Down"), "", N_("Moves the cursor to the staff below  without altering selection"), G_CALLBACK (movetostaffdown_cb)},

{
"MeasureLeft", NULL, N_("Measure Left"), "", N_("Moves the cursor to the first object in the next measure, extending selection if any"), G_CALLBACK (measureleft_cb)},

{
"MeasureRight", NULL, N_("Measure Right"), "", N_("Moves the cursor to the first object in the previous measure, extending selection if any"), G_CALLBACK (measureright_cb)},

{
"MoveToMeasureLeft", NULL, N_("Move to Measure Left"), "", N_("Moves the cursor to the first object in the next measure leaving selection, if any, unchanged"), G_CALLBACK (movetomeasureleft_cb)},

{
"MoveToMeasureRight", NULL, N_("Move to Measure Right"), "", N_("Moves the cursor to the first object in the previous measure leaving selection, if any, unchanged"), G_CALLBACK (movetomeasureright_cb)},

{
"A", NULL, N_("Change/Append A"), "", N_("Changes the note the cursor is on to the note A.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), G_CALLBACK (go_to_A_key_cb)},

{
"B", NULL, N_("Change/Append B"), "", N_("Changes the note the cursor is on to the note B.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), G_CALLBACK (go_to_B_key_cb)},

{
"C", NULL, N_("Change/Append C"), "", N_("Changes the note the cursor is on to the note C.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), G_CALLBACK (go_to_C_key_cb)},

{
"D", NULL, N_("Change/Append D"), "", N_("Changes the note the cursor is on to the note D.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), G_CALLBACK (go_to_D_key_cb)},

{
"E", NULL, N_("Change/Append E"), "", N_("Changes the note the cursor is on to the note E.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), G_CALLBACK (go_to_E_key_cb)},

{
"F", NULL, N_("Change/Append F"), "", N_("Changes the note the cursor is on to the note F.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), G_CALLBACK (go_to_F_key_cb)},

{
"G", NULL, N_("Change/Append G"), "", N_("Changes the note the cursor is on to the note G.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), G_CALLBACK (go_to_G_key_cb)},

{
"OctaveUp", NULL, N_("Octave Up"), "", N_("Changes the note at the cursor to an octave higher"), G_CALLBACK (octave_up_key_cb)},

{
"OctaveDown", NULL, N_("Octave Down"), "", N_("Changes the note at the cursor to an octave lower"), G_CALLBACK (octave_down_key_cb)},

{
"WholeNote", NULL, N_("WholeNote"), "", N_("Insert \xF0\x9D\x85\x9D"), G_CALLBACK (insert_chord_0key_cb)},

{
"HalfNote", NULL, N_("HalfNote"), "", N_("Insert \xF0\x9D\x85\x9E"), G_CALLBACK (insert_chord_1key_cb)},

{
"QuarterNote", NULL, N_("QuarterNote"), "", N_("Insert \xF0\x9D\x85\x9F"), G_CALLBACK (insert_chord_2key_cb)},

{
"EighthNote", NULL, N_("EighthNote"), "", N_("Insert \xF0\x9D\x85\xA0"), G_CALLBACK (insert_chord_3key_cb)},

{
"SixteenthNote", NULL, N_("SixteenthNote"), "", N_("Insert \xF0\x9D\x85\xA1"), G_CALLBACK (insert_chord_4key_cb)},

{
"ThirtysecondNote", NULL, N_("ThirtysecondNote"), "", N_("Insert \xF0\x9D\x85\xA2"), G_CALLBACK (insert_chord_5key_cb)},

{
"SixtyfourthNote", NULL, N_("SixtyfourthNote"), "", N_("Insert \xF0\x9D\x85\xA3"), G_CALLBACK (insert_chord_6key_cb)},

{
"OneHundredTwentyEighthNote", NULL, N_("OneHundredTwentyEighthNote"), "", N_("Insert \xF0\x9D\x85\xA4"), G_CALLBACK (insert_chord_7key_cb)},

{
"TwoHundredFiftySixthNote", NULL, N_("TwoHundredFiftySixthNote"), "", N_("Insert \xF0\x9D\x85\xA5"), G_CALLBACK (insert_chord_8key_cb)},

{
"InsertWholeRest", NULL, N_("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBB</span>"), "", N_("Insert \xF0\x9D\x84\xBB rest"), G_CALLBACK (insert_rest_0key_cb)},

{
"InsertHalfRest", NULL, N_("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBC</span>"), "", N_("Insert \xF0\x9D\x84\xBC rest"), G_CALLBACK (insert_rest_1key_cb)},

{
"InsertQuarterRest", NULL, N_("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBD</span>"), "", N_("Insert \xF0\x9D\x84\xBD rest"), G_CALLBACK (insert_rest_2key_cb)},

{
"InsertEighthRest", NULL, N_("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBE</span>"), "", N_("Insert \xF0\x9D\x84\xBE rest"), G_CALLBACK (insert_rest_3key_cb)},

{
"InsertSixteenthRest", NULL, N_("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBF</span>"), "", N_("Insert \xF0\x9D\x84\xBF rest"), G_CALLBACK (insert_rest_4key_cb)},

{
"InsertThirtysecondRest", NULL, N_("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x85\x80</span>"), "", N_("Insert \xF0\x9D\x85\x80 rest"), G_CALLBACK (insert_rest_5key_cb)},

{
"InsertSixtyfourthRest", NULL, N_("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x85\x81</span>"), "", N_("Insert \xF0\x9D\x85\x81 rest"), G_CALLBACK (insert_rest_6key_cb)},

{
"InsertBlankWholeNote", NULL, N_("Insert a \xF0\x9D\x84\xBB Spacer"), "", N_("Insert a non-printing \xF0\x9D\x84\xBB rest"), G_CALLBACK (insert_blankchord_0key_cb)},

{
"InsertBlankHalfNote", NULL, N_("Insert a \xF0\x9D\x84\xBC Spacer"), "", N_("Insert a non-printing \xF0\x9D\x84\xBC rest"), G_CALLBACK (insert_blankchord_1key_cb)},

{
"InsertBlankQuarterNote", NULL, N_("Insert a \xF0\x9D\x84\xBD Spacer"), "", N_("Insert a non-printing \xF0\x9D\x84\xBD rest"), G_CALLBACK (insert_blankchord_2key_cb)},

{
"InsertBlankEighthNote", NULL, N_("Insert a \xF0\x9D\x84\xBE Spacer"), "", N_("Insert a non-printing \xF0\x9D\x84\xBE rest"), G_CALLBACK (insert_blankchord_3key_cb)},

{
"InsertBlankSixteenthNote", NULL, N_("Insert a \xF0\x9D\x84\xBF Spacer"), "", N_("Insert a non-printing \xF0\x9D\x84\xBF rest"), G_CALLBACK (insert_blankchord_4key_cb)},

{
"InsertBlankThirtysecondNote", NULL, N_("Insert a \xF0\x9D\x85\x80 Spacer"), "", N_("Insert a non-printing \xF0\x9D\x85\x80 rest"), G_CALLBACK (insert_blankchord_5key_cb)},

{
"InsertBlankSixtyfourthNote", NULL, N_("Insert a \xF0\x9D\x85\x81 Spacer"), "", N_("Insert a non-printing \xF0\x9D\x85\x81 rest"), G_CALLBACK (insert_blankchord_6key_cb)},

{
"InsertBlankOneHundredTwentyEighthNote", NULL, N_("Insert a \xF0\x9D\x85\x82 Spacer"), "", N_("Insert a non-printing \xF0\x9D\x85\x82 rest"), G_CALLBACK (insert_blankchord_7key_cb)},

{
"InsertBlankTwoHundredFiftySixthNote", NULL, N_("Insert a \xF0\x9D\x85\x83 Spacer"), "", N_("Insert a non-printing \xF0\x9D\x85\x83 rest"), G_CALLBACK (insert_blankchord_8key_cb)},

{
"ToggleRestMode", NULL, N_("Toggle Rest Mode"), "", N_("No Tooltip yet"), G_CALLBACK (rest_toggle_key_cb)},

{
"ToggleBlankMode", NULL, N_("Toggle Blank Mode"), "", N_("No Tooltip yet"), G_CALLBACK (toggle_blank_cb)},

{
"InsertDuplet", NULL, N_("Insert Duplet"), "", N_("No Tooltip yet"), G_CALLBACK (insert_duplet_cb)},

{
"InsertTriplet", NULL, N_("Insert Triplet"), "", N_("Inserts a Start Triplet object and an End Tuplet object and places the cursor between these two"), G_CALLBACK (insert_triplet_cb)},

{
"StartTriplet", NULL, N_("Start Triplet"), "", N_("Inserts an Start Triplet object, which makes the notes following take 2/3 of their written duration. Later in this measure there should be an End Tuplet object."), G_CALLBACK (start_triplet_cb)},

{
"EndTuplet", NULL, N_("End Tuplet"), "", N_("Inserts an End Tuplet object, which terminates a tuplet started earlier in this measure."), G_CALLBACK (end_tuplet_cb)},

{
"InsertQuadtuplet", NULL, N_("Insert Quadruplet"), "", N_("No Tooltip yet"), G_CALLBACK (insert_quadtuplet_cb)},

{
"InsertQuintuplet", NULL, N_("Insert Quintuplet"), "", N_("No Tooltip yet"), G_CALLBACK (insert_quintuplet_cb)},

{
"InsertSextuplet", NULL, N_("Insert Sextuplet"), "", N_("No Tooltip yet"), G_CALLBACK (insert_sextuplet_cb)},

{
"InsertSeptuplet", NULL, N_("Insert Septuplet"), "", N_("No Tooltip yet"), G_CALLBACK (insert_septuplet_cb)},

{
"AddNoteToChord", NULL, N_("Add note"), "", N_("Add a note to the current chord\nThe cursor position determines which note to add"), G_CALLBACK (add_tone_key_cb)},

{
"RemoveNoteFromChord", NULL, N_("Remove note"), "", N_("Remove a note from the current chord, based on the cursor position"), G_CALLBACK (remove_tone_key_cb)},

{
"Sharpen", NULL, N_("Sharpen"), "", N_("Sharpen the note at the cursor"), G_CALLBACK (sharpen_key_cb)},

{
"Flatten", NULL, N_("Flatten"), "", N_("Flatten the note at the cursor"), G_CALLBACK (flatten_key_cb)},

{
"PendingSharpen", NULL, N_("Sharpen Next Note"), "", N_("Increases the sharpness of the next entered note. The status bar shows the current state."), G_CALLBACK (pending_sharpen_cb)},

{
"PendingFlatten", NULL, N_("Flatten Next Note"), "", N_("Increases the flatness of the next entered note. The status bar shows the current state."), G_CALLBACK (pending_flatten_cb)},

{
"StemUp", NULL, N_("StemUp"), "", N_("Alters a StemNeutral object to stem up."), G_CALLBACK (stem_up_cb)},

{
"StemDown", NULL, N_("StemDown"), "", N_("Alters a StemNeutral object to stem down."), G_CALLBACK (stem_down_cb)},

{
"AddDot", NULL, N_("Add Dot"), "", N_("Lengthen the chord, note or rest at the cursor by dotting it."), G_CALLBACK (add_dot_key_cb)},

{
"RemoveDot", NULL, N_("Remove Dot"), "", N_("Reduce the dotting of the chord note or rest at the cursor."), G_CALLBACK (remove_dot_key_cb)},

{
"InsertTiedNote", NULL, N_("Tied note"), "", N_("Inserts a duplicate of the current note, tied"), G_CALLBACK (tie_notes_key_cb)},

{
"ToggleTie", NULL, N_("Toggle Tie"), "", N_("Ties/unties the note at the cursor"), G_CALLBACK (toggle_tie)},

{
"DeleteObject", NULL, N_("Delete Object"), "", N_("Delete the object at the cursor"), G_CALLBACK (deleteobject_cb)},

{
"DeletePreviousObject", NULL, N_("Delete Previous Object"), "", N_("Delete to the left of the cursor."), G_CALLBACK (deletepreviousobject_cb)},

{
"InsertMeasure", NULL, N_("Insert Measure Before"), "", N_("Insert a blank measure before the current one (in all staffs)"), G_CALLBACK (insert_measure_key_cb)},

{
"AddMeasure", NULL, N_("Insert Measure After"), "", N_("Insert a blank measure after the current one (in all staffs)"), G_CALLBACK (addmeasureafter_cb)},

{
"InsertMeasureBefore", NULL, N_("Staff Insert Measure Before"), "", N_("Insert a blank measure before the current one (in current staff)"), G_CALLBACK (insertmeasurebefore_cb)},

{
"InsertMeasureAfter", NULL, N_("Staff Insert Measure After"), "", N_("Insert a blank measure in current staff after the current measure"), G_CALLBACK (insertmeasureafter_cb)},

{
"AppendMeasure", NULL, N_("Staff Append Measure"), "", N_("Append an empty measure at the end of the current staff"), G_CALLBACK (append_measure_key_cb)},

{
"DeleteMeasure", NULL, N_("Staff Delete Measure"), "", N_("Delete the current measure in this staff, leaving the staff short"), G_CALLBACK (deletemeasure_cb)},

{
"DeleteMeasureAllStaffs", NULL, N_("Delete Measure All Staffs"), "", N_("Delete the current measure in all staffs"), G_CALLBACK (deletemeasureallstaffs_cb)},

{
"ShrinkMeasures", NULL, N_("Shrink Measure"), "", N_("No Tooltip yet"), G_CALLBACK (adjust_measure_less_width_key_cb)},

{
"WidenMeasures", NULL, N_("Widen Measures"), "", N_("No Tooltip yet"), G_CALLBACK (adjust_measure_more_width_key_cb)},

{
"ShorterStaffs", NULL, N_("Shorter Staffs"), "", N_("No Tooltip yet"), G_CALLBACK (adjust_staff_less_height_key_cb)},

{
"TallerStaffs", NULL, N_("Taller Staffs"), "", N_("No Tooltip yet"), G_CALLBACK (adjust_staff_more_height_key_cb)},

{
"InsertTrebleClef", NULL, N_("New Treble Clef"), "", N_("No Tooltip yet"), G_CALLBACK (newcleftreble_cb)},

{
"InsertBassClef", NULL, N_("New Bass Clef"), "", N_("No Tooltip yet"), G_CALLBACK (newclefbass_cb)},

{
"Insertg8clef", NULL, N_("New G8 Clef"), "", N_("No Tooltip yet"), G_CALLBACK (newclefg8_cb)},

{
"InsertAltoClef", NULL, N_("New Alto Clef"), "", N_("No Tooltip yet"), G_CALLBACK (newclefalto_cb)},

{
"InsertTenorClef", NULL, N_("New Tenor Clef"), "", N_("No Tooltip yet"), G_CALLBACK (newcleftenor_cb)},

{
"InsertSopranoClef", NULL, N_("New Soprano Clef"), "", N_("No Tooltip yet"), G_CALLBACK (newclefsoprano_cb)},

{
"SetInitialTrebleClef", NULL, N_("Set Treble Clef"), "", N_("No Tooltip yet"), G_CALLBACK (setcleftreble_cb)},

{
"SetInitialBassClef", NULL, N_("Set Bass Clef"), "", N_("No Tooltip yet"), G_CALLBACK (setclefbass_cb)},

{
"SetInitialg8clef", NULL, N_("Set G8 Clef"), "", N_("No Tooltip yet"), G_CALLBACK (setclefg8_cb)},

{
"SetInitialAltoClef", NULL, N_("Set Alto Clef"), "", N_("No Tooltip yet"), G_CALLBACK (setclefalto_cb)},

{
"SetInitialTenorClef", NULL, N_("Set Tenor Clef"), "", N_("No Tooltip yet"), G_CALLBACK (setcleftenor_cb)},

{
"SetInitialSopranoClef", NULL, N_("Set Soprano Clef"), "", N_("No Tooltip yet"), G_CALLBACK (setclefsoprano_cb)},

{
"Insert22Time", NULL, N_("Insert 2/2 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig22_cb)},

{
"Insert32Time", NULL, N_("Insert 3/2 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig32_cb)},

{
"Insert42Time", NULL, N_("Insert 4/2 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig42_cb)},

{
"Insert44Time", NULL, N_("Insert 4/4 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig44_cb)},

{
"Insert34Time", NULL, N_("Insert 3/4 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig34_cb)},

{
"Insert24Time", NULL, N_("Insert 2/4 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig24_cb)},

{
"Insert64Time", NULL, N_("Insert 6/4 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig64_cb)},

{
"Insert38Time", NULL, N_("Insert 3/8 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig38_cb)},

{
"Insert68Time", NULL, N_("Insert 6/8 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig68_cb)},

{
"Insert128Time", NULL, N_("Insert 12/8 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig128_cb)},

{
"Insert98Time", NULL, N_("Insert 9/8 Time"), "", N_("No Tooltip yet"), G_CALLBACK (newtimesig98_cb)},

{
"Set22Time", NULL, N_("Set 2/2 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig22_cb)},

{
"Set32Time", NULL, N_("Set 3/2 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig32_cb)},

{
"Set42Time", NULL, N_("Set 4/2 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig42_cb)},

{
"Set44Time", NULL, N_("Set 4/4 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig44_cb)},

{
"Set34Time", NULL, N_("Set 3/4 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig34_cb)},

{
"Set24Time", NULL, N_("Set 2/4 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig24_cb)},

{
"Set64Time", NULL, N_("Set 6/4 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig64_cb)},

{
"Set38Time", NULL, N_("Set 3/8 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig38_cb)},

{
"Set68Time", NULL, N_("Set 6/8 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig68_cb)},

{
"Set128Time", NULL, N_("Set 12/8 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig128_cb)},

{
"Set98Time", NULL, N_("Set 9/8 Time"), "", N_("No Tooltip yet"), G_CALLBACK (settimesig98_cb)},

{
"InsertCmaj", NULL, N_("Insert Cmaj"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigcmaj_cb)},

{
"InsertGmaj", NULL, N_("Insert Gmaj"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysiggmaj_cb)},

{
"InsertDmaj", NULL, N_("Insert Dmaj"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigdmaj_cb)},

{
"InsertAmaj", NULL, N_("Insert Amaj"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigamaj_cb)},

{
"InsertEmaj", NULL, N_("Insert Emaj"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigemaj_cb)},

{
"InsertBmaj", NULL, N_("Insert Bmaj"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigbmaj_cb)},

{
"InsertFSharpmaj", NULL, N_("Insert F# Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigfsharpmaj_cb)},

{
"InsertCSharpmaj", NULL, N_("Insert C# Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigcsharpmaj_cb)},

{
"InsertFmaj", NULL, N_("Insert F Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigfmaj_cb)},

{
"InsertBflatmaj", NULL, N_("Insert Bb Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigbflatmaj_cb)},

{
"InsertEflatmaj", NULL, N_("Insert Eb Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigeflatmaj_cb)},

{
"InsertAflatmaj", NULL, N_("Insert Ab Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigaflatmaj_cb)},

{
"InsertDflatmaj", NULL, N_("Insert Db Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigdflatmaj_cb)},

{
"InsertGflatmaj", NULL, N_("Insert Gb Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysiggflatmaj_cb)},

{
"InsertCflatmaj", NULL, N_("Insert Cb Major"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigcflatmaj_cb)},

{
"InsertAmin", NULL, N_("Insert A Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigamin_cb)},

{
"InsertEmin", NULL, N_("Insert E Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigemin_cb)},

{
"InsertBmin", NULL, N_("Insert B Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigbmin_cb)},

{
"InsertFSharpmin", NULL, N_("Insert F# Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigfsharpmin_cb)},

{
"InsertCSharpmin", NULL, N_("Insert C# Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigcsharpmin_cb)},

{
"InsertGSharpmin", NULL, N_("Insert G# Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysiggsharpmin_cb)},

{
"InsertDSharpmin", NULL, N_("Insert D# Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigdsharpmin_cb)},

{
"InsertASharpmin", NULL, N_("Insert A# Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigasharpmin_cb)},

{
"InsertDmin", NULL, N_("Insert D Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigdmin_cb)},

{
"InsertGmin", NULL, N_("Insert G Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysiggmin_cb)},

{
"InsertCmin", NULL, N_("Insert C Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigcmin_cb)},

{
"InsertFmin", NULL, N_("Insert F Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigfmin_cb)},

{
"InsertBflatmin", NULL, N_("Insert Bb Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigbflatmin_cb)},

{
"InsertEflatmin", NULL, N_("Insert Eb Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigeflatmin_cb)},

{
"InsertAflatmin", NULL, N_("Insert Ab Minor"), "", N_("No Tooltip yet"), G_CALLBACK (newkeysigaflatmin_cb)},

{
"SetInitialCmaj", NULL, N_("Set Initial Keysig to C Major"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigcmaj_cb)},

{
"SetInitialGmaj", NULL, N_("Set Initial Keysig to G Major"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysiggmaj_cb)},

{
"SetInitialDmaj", NULL, N_("Set D Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigdmaj_cb)},

{
"SetInitialAmaj", NULL, N_("Set A Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigamaj_cb)},

{
"SetInitialEmaj", NULL, N_("Set E Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigemaj_cb)},

{
"SetInitialBmaj", NULL, N_("Set B Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigbmaj_cb)},

{
"SetInitialFSharpmaj", NULL, N_("Set F# Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigfsharpmaj_cb)},

{
"SetInitialCSharpmaj", NULL, N_("Set C# Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigcsharpmaj_cb)},

{
"SetInitialFmaj", NULL, N_("Set F Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigfmaj_cb)},

{
"SetInitialBflatmaj", NULL, N_("Set Bb Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigbflatmaj_cb)},

{
"SetInitialEflatmaj", NULL, N_("Set Eb Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigeflatmaj_cb)},

{
"SetInitialAflatmaj", NULL, N_("Set Ab Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigaflatmaj_cb)},

{
"SetInitialDflatmaj", NULL, N_("Set Db Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigdflatmaj_cb)},

{
"SetInitialGflatmaj", NULL, N_("Set Gb Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysiggflatmaj_cb)},

{
"SetInitialCflatmaj", NULL, N_("Set Cb Major as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigcflatmaj_cb)},

{
"SetInitialAmin", NULL, N_("Set A Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigamin_cb)},

{
"SetInitialEmin", NULL, N_("Set E Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigemin_cb)},

{
"SetInitialBmin", NULL, N_("Set B Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigbmin_cb)},

{
"SetInitialFSharpmin", NULL, N_("Set F# Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigfsharpmin_cb)},

{
"SetInitialCSharpmin", NULL, N_("Set C# Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigcsharpmin_cb)},

{
"SetInitialGSharpmin", NULL, N_("Set G# Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysiggsharpmin_cb)},

{
"SetInitialDSharpmin", NULL, N_("Set D# Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigdsharpmin_cb)},

{
"SetInitialASharpmin", NULL, N_("Set A# Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigasharpmin_cb)},

{
"SetInitialDmin", NULL, N_("Set D Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigdmin_cb)},

{
"SetInitialGmin", NULL, N_("Set G Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysiggmin_cb)},

{
"SetInitialCmin", NULL, N_("Set C Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigcmin_cb)},

{
"SetInitialFmin", NULL, N_("Set F Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigfmin_cb)},

{
"SetInitialBflatmin", NULL, N_("Set Bb Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigbflatmin_cb)},

{
"SetInitialEflatmin", NULL, N_("Set Eb Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigeflatmin_cb)},

{
"SetInitialAflatmin", NULL, N_("Set Ab Minor as Initial Keysig"), "", N_("No Tooltip yet"), G_CALLBACK (setkeysigaflatmin_cb)},

{
"SetMark", NULL, N_("Set Mark"), "", N_("Sets the start point for a selection,\nthe end point of the selection is unaltered"), G_CALLBACK (set_mark_cb)},

{
"UnsetMark", NULL, N_("Unset Mark"), "", N_("Gets rid of the selection."), G_CALLBACK (unset_mark_cb)},

{
"SetPoint", NULL, N_("Set Point"), "", N_("Extends the selection to the current cursor position"), G_CALLBACK (set_point_cb)},

{
"ToggleBeginSlur", NULL, N_("Begin Slur"), "", N_("Insert/delete begin slur on this note"), G_CALLBACK (toggle_begin_slur_cb)},

{
"ToggleEndSlur", NULL, N_("End Slur"), "", N_("Insert/delete end slur on this note"), G_CALLBACK (toggle_end_slur_cb)},

{
"ToggleStartCrescendo", NULL, N_("Start Crescendo (Off/On)"), "", N_("Marks/Unmarks the chord or note at the cursor as the start of a crescendo."), G_CALLBACK (toggle_start_crescendo_cb)},

{
"ToggleEndCrescendo", NULL, N_("End Crescendo (Off/On)"), "", N_("Marks/Unmarks the chord or note at the cursor as the end of a crescendo."), G_CALLBACK (toggle_end_crescendo_cb)},

{
"ToggleStartDiminuendo", NULL, N_("Start Diminuendo (Off/On)"), "", N_("Marks/Unmarks the chord or note at the cursor as the start of a diminuendo."), G_CALLBACK (toggle_start_diminuendo_cb)},

{
"ToggleEndDiminuendo", NULL, N_("End Diminuendo (Off/On)"), "", N_("Marks/Unmarks the chord or note at the cursor as the end of a diminuendo."), G_CALLBACK (toggle_end_diminuendo_cb)},

{
"ToggleGrace", NULL, N_("Grace Note Off/On"), "", N_("Makes the note at the cursor an appogiatura grace note, if it is one, makes it normal"), G_CALLBACK (toggle_grace)},

{
"ToggleAcciaccatura", NULL, N_("Acciaccatura Off/On"), "", N_("Makes the note at the cursor an acciaccatura grace note, if it is one, makes it normal"), G_CALLBACK (toggle_acciaccatura)},

{
"ForceCaution", NULL, N_("Force Cautionary Accidental"), "", N_("Give a cautionary accidental to the note at the cursor"), G_CALLBACK (force_cautionary_cb)},

{
"ChangePitch", NULL, N_("Change Pitch"), "", N_("Changes the pitch of the note at the cursor to the cursor height"), G_CALLBACK (change_pitch_cb)},

{
"InsertRhythm", NULL, N_("Insert Snippet"), "", N_("No Tooltip yet"), G_CALLBACK (insert_rhythm_pattern_cb)},

{
"NextRhythm", NULL, N_("Next Snippet"), "", N_("Make next snippet\nthe current snippet.\nNotes entered will follow the rhythmic pattern of this snippet"), G_CALLBACK (nextrhythm_cb)},

{
"AppendMeasureAllStaffs", NULL, N_("Append Measure All Staffs"), "", N_("Appends a blank measure to every staff in this movement"), G_CALLBACK (append_measure_score_cb)},

{
"ExecuteScheme", NULL, N_("Execute Scheme"), "", N_("Execute the scheme code from the scripting window"), G_CALLBACK (execute_scheme)},

{
"FileMenu", NULL, N_("File"), "", N_("Menu:\nCreating, saving, loading, exporting, importing, typesetting and printing musical scores")},

{
"Bookmarks", NULL, N_("Bookmarks"), "", N_("Creating, saving places in musical scores")},

{
"ModeMenu", NULL, N_("Mode"), "", N_("Different keyboard and MIDI entry modes")},

{
"EditMenu", NULL, N_("Edit"), "", N_("Menu:\nGeneral editing commands")},

{
"ViewMenu", NULL, N_("View"), "", N_("Menu:\nControl which tools are to be shown")},

{
"StaffMenu", NULL, N_("Staffs/Voices"), "", N_("Menu:\nCommands that manipulate Staffs and voices on staffs.\nCommands that apply to the current staff.")},

{
"Voices", NULL, N_("Voices"), "", N_("Menu:\nThings that manipulate Voices, that is separately stemmed and beamed parts that will be typeset on the same staff.\nKeep them displayed on separate staffs for editing purposes at least.\nThe typesetter will amalgamate them for you.\nSee also commands for hiding rests and moving notes and rests to avoid collisions in the Notes/Rests menu")},

{
"MovementMenu", NULL, N_("Movements"), "", N_("Menu:\nCommands that apply to the current movement in your score.\nMovements in a score could be songs in a song book etc\nAny piece of continuous music.")},

{
"HelpMenu", NULL, N_("Help"), "", N_("Menu:\nInformation and help for Denemo")},

{
"InputMenu", NULL, N_("Input"), "", N_("Menu:\nChoose whether to get input audio (e.g. microphone) or MIDI controller (e.g. MIDI keyboard)\nor just from the pc keyboard.")},

{
"SharpenEnharmonicSet", NULL, N_("Shift Accidentals Sharpwise"), "", N_("Shifts the set of accidentals one step sharper"), G_CALLBACK (set_sharper)},

{
"FlattenEnharmonicSet", NULL, N_("Shift Accidentals Flatwise"), "", N_("Shifts the set of accidentals one step flatter"), G_CALLBACK (set_flatter)},

{
"PlaybackMenu", NULL, N_("Playback"), "", N_("Music playback. Music is played between the playback start (green bar) and playback end (red bar).\nThere are playback controls (See View->Playback Controls) which make it easy to set and re-set these and also to loop-play, choose the temperament to play in etc. Or use items in this menu to play the whole piece from the cursor to the end.")},

{
"New", GTK_STOCK_NEW, N_("Empty Score"), "", N_("Start a new musical score"), G_CALLBACK (file_newwrapper)},

{
"NewScore", GTK_STOCK_NEW, N_("New"), "", N_("Start a new musical score for a named instrument/voice."), G_CALLBACK (new_score_cb)},

{
"OpenMenu", GTK_STOCK_OPEN, N_("Open"), "", N_("Menu:\nOpen a previously edited score, or a source file for transcription")},

{
"Open", GTK_STOCK_OPEN, N_("Open"), "", N_("Open a file containing a music score for editing"), G_CALLBACK (file_open_with_check)},

{
"ImportLilypond", GTK_STOCK_OPEN, N_("Import Lilypond"), "", N_("Import a Lilypond file"), G_CALLBACK (file_import_lilypond_with_check)},

{
"ImportMidi", GTK_STOCK_OPEN, N_("Import Midi"), "", N_("Import a Midi file"), G_CALLBACK (file_import_midi_with_check)},

{
"ImportMusicXml", GTK_STOCK_OPEN, N_("Import MusicXml"), "", N_("Import a MusicXml file"), G_CALLBACK (file_import_musicxml_with_check)},

{
"AddStaffs", GTK_STOCK_OPEN, N_("Add Staffs"), "", N_("Add staffs from a Denemo file"), G_CALLBACK (file_add_staffs)},

{
"AddMovements", GTK_STOCK_OPEN, N_("Add Movement"), "", N_("Add movements from a Denemo file"), G_CALLBACK (file_add_movements)},

{
"MovementProps", GTK_STOCK_PROPERTIES, N_("Change Properties"), "", N_("Change properties of this movement"), G_CALLBACK (movement_props_dialog)},

{
"OpenNewWindow", GTK_STOCK_OPEN, N_("Open In New"), "", N_("Open a file containing a music score for editing in a separate working area (tab)"), G_CALLBACK (openinnew)},

{
"Save", GTK_STOCK_SAVE, N_("Save"), "", N_("Save the score. The score is saved to disk in XML format."), G_CALLBACK (file_savewrapper)},

{
"SaveAs", GTK_STOCK_SAVE_AS, N_("Save As"), "", N_("Save the score under a new name"), G_CALLBACK (file_saveaswrapper)},

{
"SaveCopy", GTK_STOCK_SAVE_AS, N_("Create Copy"), "", N_("Save a copy of the score"), G_CALLBACK (file_copy_save)},

{
"OpenTemplate", GTK_STOCK_OPEN, N_("Open Template"), "", N_("Start a new score from a built-in template file"), G_CALLBACK (system_template_open_with_check)},

{
"OpenExample", GTK_STOCK_OPEN, N_("Open Example"), "", N_("Start a new score from a built-in example"), G_CALLBACK (system_example_open_with_check)},

{
"OpenMyTemplate", GTK_STOCK_OPEN, N_("Open Custom Template"), "", N_("Start a new score from one of your own template files"), G_CALLBACK (local_template_open_with_check)},

{
"SaveTemplate", GTK_STOCK_SAVE_AS, N_("Save Template"), "", N_("Save the score as a template for re-use as a starting point for new scores"), G_CALLBACK (template_save)},

{
"NewWindow", NULL, N_("New Tab"), "", N_("Create working area (tab with an empty score in it)"), G_CALLBACK (newview)},

{
"InsertMovementBefore", NULL, N_("Insert Movement Before"), "", N_("Insert a new movement before the current one"), G_CALLBACK (insert_movement_before)},

{
"InsertMovementAfter", NULL, N_("Insert Movement After"), "", N_("Insert a new movement after the current one"), G_CALLBACK (insert_movement_after)},

{
"NewMovement", NULL, N_("New Movement"), "", N_("Create a new movement, usign any default template"), G_CALLBACK (append_new_movement)},

{
"SaveMenu", GTK_STOCK_SAVE, N_("Save"), "", N_("Menu:\nSave the score\nBesides saving for later you can save the score for use as a template for future works, or save under a new name etc\nThe score is saved to disk in XML format.")},

{
"SaveParts", GTK_STOCK_SAVE_AS, N_("Save Parts"), "", N_("Save Parts: each staff becomes a file in lilypond format"), G_CALLBACK (file_savepartswrapper)},

{
"ExportMUDELA", GTK_STOCK_SAVE_AS, N_("Export Lilypond"), "", N_("Export the score as a lilypond file"), G_CALLBACK (export_mudela_action)},

{
"ExportPDF", GTK_STOCK_SAVE_AS, N_("Export PDF"), "", N_("Export the score as a PDF document file"), G_CALLBACK (export_pdf_action)},

{
"ExportPNG", GTK_STOCK_SAVE_AS, N_("Export PNG"), "", N_("Export the score as a PNG image file"), G_CALLBACK (export_png_action)},

{
"ExportMIDI", GTK_STOCK_SAVE_AS, N_("Export MIDI"), "", N_("Export the score as a MIDI file"), G_CALLBACK (export_midi_action)},

{
"PrintMenu", GTK_STOCK_PRINT, N_("Print"), "", N_("Menu:\nCommands for typesetting and then printing the music.\nA part, movement, full score or various reduced scores can be typeset.\n See also the Score Layout view for further options.")},

{
"PrintPreview", GTK_STOCK_PRINT_PREVIEW, N_("Print Preview"), "", N_("Displays the final finished score in your PDF viewer"), G_CALLBACK (printpreview_cb)},

{
"PrintView", GTK_STOCK_PRINT_PREVIEW, N_("Print Preview"), "", N_("Typesets the score\nIf you have a score layout selected it will use that\notherwise all movements staffs and lyrics are typeset by default.\nBe patient! It takes time to create a beautifully laid out score.\nOnce complete you can view and then send to your printer or to a file as a .pdf document."), G_CALLBACK (show_print_view)},

{
"PrintSelection", GTK_STOCK_PRINT, N_("Print Selection"), "", N_("Displays selected music from score in your pdf viewer"), G_CALLBACK (printselection_cb)},

{
"PrintExcerptPreview", GTK_STOCK_PRINT_PREVIEW, N_("Print Excerpt"), "", N_("Displays a musical excerpt in your image viewer"), G_CALLBACK (printexcerptpreview_cb)},

{
"PrintMovement", GTK_STOCK_PRINT, N_("Print Movement"), "", N_("Typesets the current movement and opens a print dialog"), G_CALLBACK (printmovement_cb)},

{
"Print", GTK_STOCK_PRINT, N_("Print"), "", N_("Typesets the score using LilyPond and opens a print dialog"), G_CALLBACK (printall_cb)},

{
"PrintPart", GTK_STOCK_PRINT, N_("Print Part"), "", N_("Typesets the current part (the one containing the cursor)."), G_CALLBACK (printpart_cb)},

{
"Close", GTK_STOCK_CLOSE, N_("Close Score"), "", N_("Close the current score. Other scores (tabs) will stay open"), G_CALLBACK (close_gui_with_check)},

{
"Quit", GTK_STOCK_QUIT, N_("Quit"), "", N_("Quit the Denemo program - closes tabs one at a time."), G_CALLBACK (closewrapper)},

{
"Undo", GTK_STOCK_UNDO, N_("Undo"), "", N_("Undoes one (more) step of your edits to the current score."), G_CALLBACK (undowrapper)},

{
"Redo", GTK_STOCK_REDO, N_("Redo"), "", N_("Redoes the next of the steps you have Undone"), G_CALLBACK (redowrapper)},

{
"Select", NULL, N_("Select"), "", N_("Selecting stretches of notes")},

{
"ExtendSelect", NULL, N_("Extend Selection"), "", N_("Extend the selection")},

{
"Copy", GTK_STOCK_COPY, N_("Copy"), "", N_("Copy the music selected to the Denemo clipboard"), G_CALLBACK (copywrapper)},

{
"Cut", GTK_STOCK_CUT, N_("Cut"), "", N_("Cut the music selected to the Denemo clipboard"), G_CALLBACK (cutwrapper)},

{
"Paste", GTK_STOCK_PASTE, N_("Paste"), "", N_("Paste the Denemo clipboard into the score where the cursor is positioned"), G_CALLBACK (pastewrapper)},

{
"PasteClipboard", GTK_STOCK_PASTE, N_("Paste LilyPond notes"), "", N_("Paste LilyPond notes from the text clipboard\nThis will import music written as LilyPond syntax\nYou open the LilyPond file in a texteditor, copy the stretch of notes (control-c command in your texteditor usually) and then use this command."), G_CALLBACK (paste_clipboard)},

{
"ScoreProperties", GTK_STOCK_PROPERTIES, N_("Score Properties"), "", N_("Change some of the properties of the current score. This will start up a dialog window"), G_CALLBACK (score_properties_dialog)},

{
"SaveSelection", NULL, N_("Save Selection"), "", N_("Save the selected music. Not sure if this is working"), G_CALLBACK (saveselwrapper)},

{
"Preferences", GTK_STOCK_PREFERENCES, N_("Change Preferences"), "", N_("Set and save your preferences for how Denemo operates on startup.\nAdvanced users can edit .denemo-XXXX/denemorc for missing ones"), G_CALLBACK (preferences_change)},

{
"KeyBindings", NULL, N_("Customize Commands, Shortcuts ..."), "", N_("Set actions to take in response to keypresses")},

{
"SaveAccels", GTK_STOCK_SAVE, N_("Save Command Set"), "", N_("Save the current commands and keyboard shortcuts as the default"), G_CALLBACK (save_default_keymap_file_wrapper)},

{
"CommandManagement", NULL, N_("Manage Command Set"), "", N_("View help, change and save keyboard shortcuts"), G_CALLBACK (configure_keyboard_dialog)},

{
"SwapStaffs", NULL, N_("Swap Staffs"), "", N_("Swap this staff with the one higher up.\nBe aware that if you have inserted directives to move a voice to another staff\nthese may need re-making."), G_CALLBACK (swapstaffs)},

{
"SplitVoices", NULL, N_("Split Voices"), "", N_("Split off the next voice as a separate staff"), G_CALLBACK (splitstaffs)},

{
"JoinVoices", NULL, N_("Join Voices"), "", N_("Merge this staff as a voice on the previous staff"), G_CALLBACK (joinstaffs)},

{
"SwapMovements", NULL, N_("Swap Movements"), "", N_("Swap this movement with the one before"), G_CALLBACK (swapmovements)},

{
"VoiceUp", NULL, N_("Voice Up"), "", N_("Go to the higher numbered voice on staff, extending selection if any"), G_CALLBACK (voiceup_cb)},

{
"VoiceDown", NULL, N_("Voice Down"), "", N_("Go to the lower numbered voice on this staff, extending selection if any"), G_CALLBACK (voicedown_cb)},

{
"MoveToVoiceUp", NULL, N_("Move to Voice Up"), "", N_("Go to the higher numbered voice on staff without altering selection"), G_CALLBACK (movetovoiceup_cb)},

{
"MoveToVoiceDown", NULL, N_("Move to Voice Down"), "", N_("Go to the lower numbered voice on this staff without altering selection"), G_CALLBACK (movetovoicedown_cb)},

{
"AddBefore", NULL, N_("Add Staff Before"), "", N_("Inserts a new staff before the current staff"), G_CALLBACK (newstaffbefore)},

{
"AddAfter", NULL, N_("Add Staff After"), "", N_("Inserts/Adds a new staff after the current staff"), G_CALLBACK (dnm_newstaffafter)},

{
"AddInitial", NULL, N_("Add Initial Staff"), "", N_("Inserts a new staff at the top of the score"), G_CALLBACK (newstaffinitial)},

{
"AddLast", NULL, N_("Add Last Staff"), "", N_("Inserts a new staff at the end of the score"), G_CALLBACK (newstafflast)},

{
"DeleteBefore", NULL, N_("Delete Staff Before"), "", N_("Deletes the staff before the current staff"), G_CALLBACK (delete_staff_before)},

{
"DeleteStaff", NULL, N_("Delete Current Staff"), "", N_("Deletes the current staff"), G_CALLBACK (delete_staff_current)},

{
"DeleteAfter", NULL, N_("Delete Staff After"), "", N_("Deletes the staff after the current staff"), G_CALLBACK (delete_staff_after)},

{
"AddVoice", NULL, N_("Add Voice"), "", N_("Adds a new voice (part), to the current staff. It is tricky to switch between the voices. Suggest to use merge staffs"), G_CALLBACK (dnm_newstaffvoice)},

{
"StaffProperties", GTK_STOCK_PROPERTIES, N_("Staff Properties"), "", N_("Change the properties of the current staff"), G_CALLBACK (staff_properties_change_cb)},

{
"InsertMenu", NULL, N_("InsertMenu"), "", N_("Insert")},

{
"Clef", NULL, N_("Clef"), "", N_("Menu:\nClef")},

{
"InitialClef", NULL, N_("Initial Clef"), "", N_("Change the initial clef of the current staff"), G_CALLBACK (clef_change_initial)},

{
"InsertClef", NULL, N_("Clef Change"), "", N_("Insert/Edit a change of clef at the cursor"), G_CALLBACK (clef_change_insert)},

{
"Key", NULL, N_("Keys"), "", N_("Menu:\nSet the key signature or insert a change of key signature\nSharpen or flatten a key signature or\nhide it on printing")},

{
"InitialKey", NULL, N_("Initial Key"), "", N_("Set the initial key signature of the current staff"), G_CALLBACK (key_change_initial)},

{
"InsertKey", NULL, N_("Key Signature Change"), "", N_("Insert/Edit a key change at the cursor position"), G_CALLBACK (key_change_insert)},

{
"TimeSig", NULL, N_("Time Signatures"), "", N_("Menu:\nSet the time signature, insert or edit a change of timesignature,\nhide a time signature on printing.\nControl whether to use numerical or traditional styles.")},

{
"InitialTimeSig", NULL, N_("Inital Time Signature"), "", N_("Set the initial time signature of the current staff"), G_CALLBACK (timesig_change_initial)},

{
"InsertTimeSig", NULL, N_("Time Signature Change"), "", N_("Edit/Insert a time signature change for the current measure"), G_CALLBACK (timesig_change_insert)},

{
"ChangeNotehead", NULL, N_("Set Notehead"), "", N_("Change the type of notehead for the current note"), G_CALLBACK (set_notehead)},

{
"InsertStem", NULL, N_("Auto Stemming"), "", N_("Inserts a stem neutral object. After this automatic stem directions are active. You can click on this tag and use Sharpen/StemUp etc commands to change stem direction"), G_CALLBACK (stem_directive_insert)},

{
"AddVerse", NULL, N_("Add Lyric Verse"), "", N_("Add a verse of lyrics"), G_CALLBACK (add_verse)},

{
"DeleteVerse", NULL, N_("Delete Verse"), "", N_("Deletes current verse of lyrics from current voice"), G_CALLBACK (delete_verse)},

{
"EditFiguredBass", NULL, N_("Insert/Edit Figured Bass"), "", N_("Add a bass figure to the current note. Use | sign to split the duration of a note so as to have multiple figures on one note. See Lilypond docs for other notation"), G_CALLBACK (figure_insert)},

{
"DeleteFiguredBass", NULL, N_("Delete Figures"), "", N_("Delete the figured bass on the current staff"), G_CALLBACK (delete_figured_bass)},

{
"HideFiguredBass", NULL, N_("Hide Figures (Print)"), "", N_("Hide the figured bass on the current staff on printing"), G_CALLBACK (hide_figured_bass)},

{
"ShowFiguredBass", NULL, N_("Show Figures (Print)"), "", N_("Show the figured bass on the current staff on printing"), G_CALLBACK (show_figured_bass)},

{
"EditChords", NULL, N_("Edit Chord Symbols"), "", N_("Allows chord symbols to be added to the current note. E.G.cis:dim7 for c-sharp diminished 7th. See Lilypond docs for notation"), G_CALLBACK (fakechord_insert)},

{
"InsertDynamic", NULL, N_("Insert Dynamics"), "", N_("Inserts a dynamic marking at the cursor position"), G_CALLBACK (insert_dynamic)},

{
"EditObject", NULL, N_("Edit Object"), "", N_("Edit the object at the cursor."), G_CALLBACK (edit_object)},

{
"EditDirective", NULL, N_("Edit Directives"), "", N_("Edit any directives attached to chord/note at cursor."), G_CALLBACK (edit_object_directive)},

{
"EditStaffDirective", NULL, N_("Edit Staff Directives"), "", N_("Edit any directives attached to staff."), G_CALLBACK (edit_staff_directive)},

{
"EditVoiceDirective", NULL, N_("Edit Voice Directives"), "", N_("Edit any directives attached to voice."), G_CALLBACK (edit_voice_directive)},

{
"EditScoreDirective", NULL, N_("Edit Score Directives"), "", N_("Edit any directives attached to score."), G_CALLBACK (edit_score_directive)},

{
"EditMovementDirective", NULL, N_("Edit Movement Directives"), "", N_("Edit any directives attached to movement."), G_CALLBACK (edit_movement_directive)},

{
"EditClefDirective", NULL, N_("Edit Clef Directives"), "", N_("Edit any directives attached to clef."), G_CALLBACK (edit_clef_directive)},

{
"EditTimesigDirective", NULL, N_("Edit Time Signature Directives"), "", N_("Edit any directives attached to time signature."), G_CALLBACK (edit_timesig_directive)},

{
"EditKeysigDirective", NULL, N_("Edit Key Signature Directives"), "", N_("Edit any directives attached to key signature."), G_CALLBACK (edit_keysig_directive)},

{
"DeleteDirective", NULL, N_("Delete a Directive"), "", N_("Delete a directive attached to chord/note at cursor."), G_CALLBACK (delete_chord_or_note_directive)},

{
"AttachLilyToNote", NULL, N_("Attach Lilypond to Note"), "", N_("Attach or edit attached LilyPond text to the note at the cursor. This can be used for guitar fingerings, cautionary accidentals and much more. See LilyPond documentation."), G_CALLBACK (note_directive)},

{
"AttachLilyToChord", NULL, N_("Attach Lilypond to Chord"), "", N_("Attach or edit attached LilyPond text to the chord at the cursor. This can be used for attaching and placing text and much more. See LilyPond documentation."), G_CALLBACK (chord_directive)},

{
"InsertBarline", NULL, N_("Insert Barline"), "", N_("Inserts specialized barline at the cursor position. Mostly not working"), G_CALLBACK (insert_barline)},

{
"NavigationMenu", NULL, N_("Navigation"), "", N_("Moving the cursor within the current movement, setting bookmarks, seeking things in the score")},

{
"GoToMeasure", NULL, N_("Go to Measure"), "", N_("Opens a dialog for going to a numbered measure"), G_CALLBACK (tomeasurenum)},

{
"GoToBeginning", GTK_STOCK_GOTO_FIRST, N_("Go to Beginning"), "", N_("Cursor to start of staff/voice, extending selection if any"), G_CALLBACK (tohome)},

{
"GoToEnd", GTK_STOCK_GOTO_LAST, N_("Go to End"), "", N_("Cursor to end of staff/voice, extending selection if any"), G_CALLBACK (toend)},

{
"MoveToBeginning", GTK_STOCK_GOTO_FIRST, N_("Move to Staff/Voice Beginning"), "", N_("Cursor to start of staff/voice, without extending selection if any"), G_CALLBACK (movetostart)},

{
"MoveToEnd", GTK_STOCK_GOTO_LAST, N_("Move to Staff/Voice End"), "", N_("Cursor to end of staff/voice, without extending selection if any"), G_CALLBACK (movetoend)},

{
"NextMovement", NULL, N_("Next Movement"), "", N_("Go to the next movement"), G_CALLBACK (next_movement)},

{
"PreviousMovement", NULL, N_("Previous Movement"), "", N_("Go to the previous movement"), G_CALLBACK (prev_movement)},

{
"DeleteMovement", NULL, N_("Delete Movement"), "", N_("Delete the current movement"), G_CALLBACK (delete_movement)},

{
"Play", GTK_STOCK_MEDIA_PLAY, N_("Play"), "", N_("Play"), G_CALLBACK (ext_midi_playback)},

{
"Stop", GTK_STOCK_MEDIA_STOP, N_("Stop"), "", N_("Stop"), G_CALLBACK (stop_midi_playback)},

{
"PlaybackProperties", GTK_STOCK_PROPERTIES, N_("Playback Properties"), "", N_("Allows you to specify properties used in playing back (midi)"), G_CALLBACK (playback_properties_change)},

{
"Help", NULL, N_("Browse Manual"), "", N_("Opens a browser on the user manual"), G_CALLBACK (browse_manual)},

{
"About", NULL, N_("About"), "", N_("Gives the version number etc of this program"), G_CALLBACK (about)},

{
"MoreMenu", NULL, N_("More"), "", N_("Allows choosing extra commands/menu items from disk")},

{
"MoreCommands", NULL, N_("More Commands"), "", N_("Allows choosing standard extra commands/menu items"), G_CALLBACK (morecommands)},

{
"MyCommands", NULL, N_("My Commands"), "", N_("Allows choosing extra commands/menu items from your own collection of extras"), G_CALLBACK (mycommands)},

{
"FetchCommands", NULL, N_("Update Commands from Internet"), "", N_("Refreshes the set of commands available from Denemo.org.\nUse More Commands after this has finished"), G_CALLBACK (fetchcommands)},

{
"OpenRecent", GTK_STOCK_OPEN, N_("Open Recent"), "", N_("Open previously used files")},

{
"Import", GTK_STOCK_OPEN, N_("Import File"), "", N_("Menu:\nImport the supported file formats")},

{
"Export", NULL, N_("Export As"), "", N_("Menu:\nExport the music to another file format.\nThese file formats will not support all the features of the score.")},

{
"ToggleEdit", NULL, N_("Toggle Edit Mode"), "", N_("Toggle between current mode and edit mode"), G_CALLBACK (toggle_edit_mode)},

{
"ToggleRest", NULL, N_("Toggle Rest Mode"), "", N_("Toggle between note entry and rest entry"), G_CALLBACK (toggle_rest_mode)},

{
"ToggleRhythm", NULL, N_("Toggle Audible Feedback"), "", N_("Toggle audible feedback on/off"), G_CALLBACK (toggle_rhythm_mode)},

{
"ClearOverlay", NULL, N_("Clear Overlay"), "", N_("Clear the list of pitches that overlay the notes"), G_CALLBACK (clear_overlay)},

{
"CreateRhythm", NULL, N_("Create Snippet"), "", N_("Copy selection as music snippet or rhythm pattern for notes to follow as they are entered"), G_CALLBACK (create_rhythm_cb)},

{
"DeleteRhythm", NULL, N_("Delete Snippet"), "", N_("Delete the selected music snippet/rhythm pattern"), G_CALLBACK (delete_rhythm_cb)},

{
"ClassicModeNote", NULL, N_("Classic Mode"), "", N_("Menu:\nMoving the cursor and inserting notes or rests there")},

{
"SelectNote", NULL, N_("Select Note"), "", N_("Menu:\nMoving the cursor to the nearest ...")},

{
"InsertModeNote", NULL, N_("Insert"), "", N_("Menu:\nCommands for notes: inserting, deleting, etc.")},

{
"StemControl", NULL, N_("Stem Direction"), "", N_("Menu:\nCommands to control the stem up/down")},

{
"NoteheadControl", NULL, N_("Notehead Types"), "", N_("Menu:\nDifferent types of notehead")},

{
"TiedNotes", NULL, N_("Tied Notes"), "", N_("Menu:\nCreating Tied Notes\nDo not confuse with slurs!!")},

{
"DisplayEffects", NULL, N_("Display Effects"), "", N_("Menu:\nEffects that are only for the Denemo display,\nnot affecting the printed page.")},

{
"Articulations", NULL, N_("Articulations"), "", N_("Menu:\nArticulations")},

{
"Slurs", NULL, N_("Slurs"), "", N_("Menu:\nSlurs\nDo not confuse with ties which in print can look the same\nSlurs have little ticks at the end in the Denemo display.")},

{
"GraceNotes", NULL, N_("Grace Notes"), "", N_("Menu:\nGrace Notes")},

{
"Score", NULL, N_("Score"), "", N_("Menu:\nCommands that affect the entire piece of music across all movements.")},

{
"Instruments", NULL, N_("Instruments"), "", N_("Menu:\nInstruments")},

{
"Markings", NULL, N_("Markings"), "", N_("Menu:\nMarkings to be applied to the note at the cursor.\nIncludes articulations, string numbers, chord symbols, arbitrary text ...")},

{
"TextMarks", NULL, N_("Text/Symbol"), "", N_("Menu:\nText and symbol Markings")},

{
"Strings", NULL, N_("Fingerings etc for Instruments"), "", N_("Menu:\nMarkings on scores for Instruments")},

{
"InsertNote", NULL, N_("Note Insertion"), "", N_("Menu:\nInserting the note ...")},

{
"Lyrics", NULL, N_("Lyrics"), "", N_("Menu:\nLyrics consist of syllables which are placed under or above the notes of the voice they are attached to\nYou can use the hyphen - and underscore _ to affect the placement\nas well as slurs placed over notes. Spaces, tabs or new lines can serve to separate the syllables, it makes no difference which.")},

{
"Directives", NULL, N_("Directives"), "", N_("Menu:\nDirectives tell Denemo to do something special with the object they are attached to\nor at the point in the music they are placed\nThis can be directing the typesetter to do something special\nor issuing MIDI instructions to alter the playback\nor even take an action, such as linking to source manuscript when clicked")},

{
"Navigation", NULL, N_("Navigation"), "", N_("Menu:\nMoving the Denemo cursor around the piece")},

{
"NoteEntry", NULL, N_("Insert Note"), "", N_("Menu:\nEntering notes")},

{
"Articulation", NULL, N_("Articulation"), "", N_("Menu:\nMarks placed on notes and chords")},

{
"Edit", NULL, N_("Edit"), "", N_("Menu:\nEditing")},

{
"Measure", NULL, N_("Measure"), "", N_("Menu:\nManipulating measures\nCommands that apply to the current measure")},

{
"Staff", NULL, N_("Staff"), "", N_("Menu:\nCommands for staffs")},

{
"Playback", NULL, N_("Playback"), "", N_("Menu:\nPlaying the music via the computer's soundcard.\nAll or part of the piece can be played controlled by the green and red markers (start and stop playing)\nUse the Playback Controls (from the View menu) to set/reset these and to loop play while editing the score")},

{
"SelectDuration", NULL, N_("Select Duration"), "", N_("Menu:\nChanging the prevailing duration or rhythm pattern")},

{
"EditModeNote", NULL, N_("Append/Edit"), "", N_("Menu:\nAppending, Changing, and deleting notes")},

{
"EditNote", NULL, N_("Append/Edit Note"), "", N_("Menu:\nAppending or Editing notes")},

{
"EditDuration", NULL, N_("Append/Insert Duration"), "", N_("Menu:\nAppending or Editing durations")},

{
"Cursor", NULL, N_("Cursor"), "", N_("Menu:\nMoving the cursor")},

{
"CursorToNote", NULL, N_("Cursor to Note"), "", N_("Menu:\nMoving the cursor to note positions")},

{
"ClefMenu", NULL, N_("Clefs"), "", N_("Menu:\nInsert/change clef, set initial clef, hide a clef on printing,\nor display in a different clef")},

{
"ChordMenu", NULL, N_("Chords"), "", N_("Menu:\nCommands for creating chords. Note that chord symbols and figured bass are under markings on notes.\nSee Notes/Rests menu")},

{
"ChordNoteMenu", NULL, N_("Add Note"), "", N_("Menu:\nAdding notes to a note or chord at the cursor to make chords")},

{
"ChordSymbols", NULL, N_("Chords Symbols"), "", N_("Menu:\nAdding Chord Symbols over music")},

{
"FiguredBass", NULL, N_("Figured Bass"), "", N_("Menu:\nAdding Figured Bass Figures")},

{
"MeasureMenu", NULL, N_("Measures"), "", N_("Menu:\nMeasures: adding, deleting, navigating etc")},

{
"Insert", NULL, N_("Insert"), "", N_("Menu:\nInserting notes, measures, staffs, keysignatures etc")},

{
"InsertStaff", NULL, N_("Add Staff"), "", N_("Menu:\nInsert a new staff postioned relative to current staff")},

{
"InsertMovement", NULL, N_("Insert Movement"), "", N_("Menu:\nInsert a new movement positioned relative to current movement")},

{
"InsertDuration", NULL, N_("Insert at Cursor"), "", N_("Menu:\nInserting notes of a given duration at cursor note height")},

{
"Change", NULL, N_("Change"), "", N_("Menu:\nChanging properties of notes, measures, staffs, keysigs etc")},

{
"ModelessNote", NULL, N_("Notes/Durations"), "", N_("Menu:\nModeless actions on notes/rests")},

{
"NotesRests", NULL, N_("Notes/Rests"), "", N_("Menu:\nCommands for notes/rests")},

{
"RestEntry", NULL, N_("Rest Insertion"), "", N_("Menu:\nModeless entry of rests")},

{
"Educational", NULL, N_("Educational"), "", N_("Menu:\nEducational Games")},

{
"EditDirectivesMenu", NULL, N_("Edit Directive(s)"), "", N_("Menu:\nEditing directives")},

{
"ChangeNote", NULL, N_("Change Note"), "", N_("Menu:\nChanging the note at the cursor to the nearest ...")},

{
"ChangeDuration", NULL, N_("Edit Duration"), "", N_("Menu:\nChanges the duration of the current note")},

{
"ChangeRest", NULL, N_("Change Rest"), "", N_("Menu:\nChanges the duration of the current rest")},

{
"ExpressionMarks", NULL, N_("Expression Marks"), "", N_("Menu:\nDynamics, staccato, slurs, ties and other expressive marks")},

{
"Dynamics", NULL, N_("Dynamics"), "", N_("Dynamic markings")},

{
"Ornaments", NULL, N_("Ornaments"), "", N_("Menu:\ngrace notes etc")},

{
"Other", NULL, N_("Other"), "", N_("Lyrics, chord symbols, figured basses etc")},

{
"Others", NULL, N_("Others"), "", N_("Less used actions")},

{
"Favorites", NULL, N_("Favorites"), "", N_("Customized LilyPond inserts. Store often-used inserts here labelled with what they do")},

{
"AddFavorite", NULL, N_("Add Favorite"), "", N_("Menu:\nAdd a custom LilyPond insert to favorites menu")},

{
"Tuplets", NULL, N_("Tuplets"), "", N_("Menu:\nEntering triplets and other tuplets")},

{
"Delete", NULL, N_("Delete"), "", N_("Menu:\nDeleting notes, measures staffs keysigs etc")},

{
"Beaming", NULL, N_("Beaming"), "", N_("Menu:\nBeaming - controlling which notes are beamed together")},

{
"Conditional Directives", NULL, N_("Conditional Directives"), "", N_("Menu:\nFor making directives apply only to one score layout or be omitted from one score layout")},

{
"MIDI", NULL, N_("MIDI Keyboard"), "", N_("Menu:\nFor affecting how MIDI in works. MIDI in refers to MIDI keyboards or other controllers connected to the computer. You may need to configure this using the Audio/MIDI section of the Preferences.")},

{
"Keyboard", NULL, N_("PC Keyboard"), "", N_("Menu:\nChoose between different ways of entering notes from the computer keyboard.")},

{
"Mouse", NULL, N_("Mouse"), "", N_("Menu:\nChoose between different ways of using the mouse.")},

{
"PrintTranspositions", NULL, N_("Print Transposed"), "", N_("Menu:\nTransposing the score when typesetting.")},

{
"Print Transpositions", NULL, N_("Start/Stop Transposing"), "", N_("Menu:\nInserting markers to start/stop transposition when typesetting. The markers affect only the music in the staff/voice they are placed in.")},

{
"Barlines", NULL, N_("Inserting Barlines"), "", N_("Menu:\nDifferent sorts of barline (repeat barlines, double bars, end of movement ...)")},

{
"Seek", NULL, N_("Seek"), "", N_("Menu:\nMoving the Denemo cursor forwards/backwards to find useful places in the score")},

{
"Titles", NULL, N_("Titles"), "", N_("Menu:\nVarious titling schemes, (main title, movement titles, composer etc) and adding table of contents, critical commentary")},

{
"PrintLayout", NULL, N_("Print Layout"), "", N_("Menu:\nCommands to affect the layout of the typeset score.")},

{
"CaptureScore", NULL, N_("Capture Score"), "", N_("Menu:\nCommands to embed images taken from original manuscripts etc into the score (one for each measure)")},

{
"Ossia", NULL, N_("Ossia"), "", N_("Menu:\nCreating an Ossia staff, which appears with alternative interpretations for a bar or two on typesetting")},

{
"Numbering", NULL, N_("Bar Numbering"), "", N_("Menu:\nCommands for setting how often bar numbers appear in the typeset version of this staff")},

{
"StaffGroupings", NULL, N_("Staff Groupings (Braces)"), "", N_("Menu:\nVarious sorts of brace linking the staffs together. These may be nested, see the Score Layout view for a display of the staff groups created.")},

{
"AddAboveBase", NULL, N_("Add Note Above Base of Chord"), "", N_("Menu:\nAdding notes to the chord at the cursor at various intervals above base note")},

{
"AddBelowTop", NULL, N_("Add Note Below Top of Chord"), "", N_("Menu:\nAdding notes to the chord at the cursor at various intervals below the top-most note")},

{
"ChangePrintDuration", NULL, N_("Change Duration of Note"), "", N_("Menu:\nCommands to include other durations in a chord (instead of polyphony - this is specialized stuff!) The note at the cursor height is altered.")},

{
"Fingerings", NULL, N_("Fingerings"), "", N_("Menu:\nFingerings (numbers) to typeset near to notes")},

{
"Transpose", NULL, N_("Transpose Music"), "", N_("Menu:\nAlters the note/chord at the cursor or the selection transposing it by the interval chosen. This alters the music in the score - see other transpose options for transposing the printed output, leaving the music untouched.")},

{
"Conditional-Directives", NULL, N_("Make a Directive Conditional"), "", N_("Menu:\nMark the directive at the cursor as applying to just the current score layout, or to exclude the current layout. Use this for example to exclude redundant first and second time markings in parts which don't need them.")},

{
"SingleRandomNote", NULL, N_("Single Random Note"), "", N_("Menu:\nTo insert random notes at cursor")},

{
"GenerateRhythmFromString", NULL, N_("Generate Rhythm from String"), "", N_("Menu:\nCreating rhythms from a sequence of characters.")},

{
"NotationMagick", NULL, N_("Notation Magick"), "", N_("Menu:\nMethods of generating music of guided randomness")},

{
"Zoom", NULL, N_("Display Zoom"), "", N_("Menu:\nZooming the Denemo display. Usually Control and mouse wheel are used for this.")},

{
"Aural Training", NULL, N_("Aural Training"), "", N_("Menu:\nGames for testing your musical ear")},

{
"Note-Reading", NULL, N_("Note Reading"), "", N_("Menu:\nGames for testing your music reading ability. Notice there is (elsewhere) the Checking Pitches feature which lets you test your ability to play via Midi Controller.")},

{
"Simple Titles", NULL, N_("Simple Titling"), "", N_("Menu:\nCommands to place titles etc at the start of the music (and each movement if needed). Do not mix with Book Titles.")},

{
"Book Titles", NULL, N_("Book Titling"), "", N_("Menu:\nCommands to create a title page, table of contents, commentaries, appendices etc.")},

#define NOTE0 "\xF0\x9D\x85\x9D"
#define REST0 "\xF0\x9D\x84\xBB"
{
"0", "NULL", NOTE0, NULL, "Inserts a note at the cursor with duration " NOTE0 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur0)},

{
"Change0", "NULL", NOTE0, NULL, "Change current note to a " NOTE0, G_CALLBACK (ChangeDur0)},

{
"Insert0", NULL, "Insert a " NOTE0 "", NULL, "Inserts a " NOTE0 " at cursor position\nSets prevailing duration to " NOTE0, G_CALLBACK (InsertDur0)},

{
"InsertRest0", NULL, "Insert a " REST0 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE0, G_CALLBACK (InsertRest0)},

{
"Set0", NULL, "Set Duration to " NOTE0 "", NULL, "Sets prevailing duration to " NOTE0 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur0)},

#define NOTE1 "\xF0\x9D\x85\x9E"
#define REST1 "\xF0\x9D\x84\xBC"
{
"1", "NULL", NOTE1, NULL, "Inserts a note at the cursor with duration " NOTE1 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur1)},

{
"Change1", "NULL", NOTE1, NULL, "Change current note to a " NOTE1, G_CALLBACK (ChangeDur1)},

{
"Insert1", NULL, "Insert a " NOTE1 "", NULL, "Inserts a " NOTE1 " at cursor position\nSets prevailing duration to " NOTE1, G_CALLBACK (InsertDur1)},

{
"InsertRest1", NULL, "Insert a " REST1 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE1, G_CALLBACK (InsertRest1)},

{
"Set1", NULL, "Set Duration to " NOTE1 "", NULL, "Sets prevailing duration to " NOTE1 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur1)},

#define NOTE2 "\xF0\x9D\x85\x9F"
#define REST2 "\xF0\x9D\x84\xBD"
{
"2", "NULL", NOTE2, NULL, "Inserts a note at the cursor with duration " NOTE2 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur2)},

{
"Change2", "NULL", NOTE2, NULL, "Change current note to a " NOTE2, G_CALLBACK (ChangeDur2)},

{
"Insert2", NULL, "Insert a " NOTE2 "", NULL, "Inserts a " NOTE2 " at cursor position\nSets prevailing duration to " NOTE2, G_CALLBACK (InsertDur2)},

{
"InsertRest2", NULL, "Insert a " REST2 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE2, G_CALLBACK (InsertRest2)},

{
"Set2", NULL, "Set Duration to " NOTE2 "", NULL, "Sets prevailing duration to " NOTE2 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur2)},

#define NOTE3 "\xF0\x9D\x85\xA0"
#define REST3 "\xF0\x9D\x84\xBE"
{
"3", "NULL", NOTE3, NULL, "Inserts a note at the cursor with duration " NOTE3 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur3)},

{
"Change3", "NULL", NOTE3, NULL, "Change current note to a " NOTE3, G_CALLBACK (ChangeDur3)},

{
"Insert3", NULL, "Insert a " NOTE3 "", NULL, "Inserts a " NOTE3 " at cursor position\nSets prevailing duration to " NOTE3, G_CALLBACK (InsertDur3)},

{
"InsertRest3", NULL, "Insert a " REST3 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE3, G_CALLBACK (InsertRest3)},

{
"Set3", NULL, "Set Duration to " NOTE3 "", NULL, "Sets prevailing duration to " NOTE3 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur3)},

#define NOTE4 "\xF0\x9D\x85\xA1"
#define REST4 "\xF0\x9D\x84\xBF"
{
"4", "NULL", NOTE4, NULL, "Inserts a note at the cursor with duration " NOTE4 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur4)},

{
"Change4", "NULL", NOTE4, NULL, "Change current note to a " NOTE4, G_CALLBACK (ChangeDur4)},

{
"Insert4", NULL, "Insert a " NOTE4 "", NULL, "Inserts a " NOTE4 " at cursor position\nSets prevailing duration to " NOTE4, G_CALLBACK (InsertDur4)},

{
"InsertRest4", NULL, "Insert a " REST4 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE4, G_CALLBACK (InsertRest4)},

{
"Set4", NULL, "Set Duration to " NOTE4 "", NULL, "Sets prevailing duration to " NOTE4 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur4)},

#define NOTE5 "\xF0\x9D\x85\xA2"
#define REST5 "\xF0\x9D\x85\x80"
{
"5", "NULL", NOTE5, NULL, "Inserts a note at the cursor with duration " NOTE5 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur5)},

{
"Change5", "NULL", NOTE5, NULL, "Change current note to a " NOTE5, G_CALLBACK (ChangeDur5)},

{
"Insert5", NULL, "Insert a " NOTE5 "", NULL, "Inserts a " NOTE5 " at cursor position\nSets prevailing duration to " NOTE5, G_CALLBACK (InsertDur5)},

{
"InsertRest5", NULL, "Insert a " REST5 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE5, G_CALLBACK (InsertRest5)},

{
"Set5", NULL, "Set Duration to " NOTE5 "", NULL, "Sets prevailing duration to " NOTE5 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur5)},

#define NOTE6 "\xF0\x9D\x85\xA3"
#define REST6 "\xF0\x9D\x85\x81"
{
"6", "NULL", NOTE6, NULL, "Inserts a note at the cursor with duration " NOTE6 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur6)},

{
"Change6", "NULL", NOTE6, NULL, "Change current note to a " NOTE6, G_CALLBACK (ChangeDur6)},

{
"Insert6", NULL, "Insert a " NOTE6 "", NULL, "Inserts a " NOTE6 " at cursor position\nSets prevailing duration to " NOTE6, G_CALLBACK (InsertDur6)},

{
"InsertRest6", NULL, "Insert a " REST6 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE6, G_CALLBACK (InsertRest6)},

{
"Set6", NULL, "Set Duration to " NOTE6 "", NULL, "Sets prevailing duration to " NOTE6 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur6)},

#define NOTE7 "\xF0\x9D\x85\xA4"
#define REST7 "\xF0\x9D\x85\x82"
{
"7", "NULL", NOTE7, NULL, "Inserts a note at the cursor with duration " NOTE7 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur7)},

{
"Change7", "NULL", NOTE7, NULL, "Change current note to a " NOTE7, G_CALLBACK (ChangeDur7)},

{
"Insert7", NULL, "Insert a " NOTE7 "", NULL, "Inserts a " NOTE7 " at cursor position\nSets prevailing duration to " NOTE7, G_CALLBACK (InsertDur7)},

{
"InsertRest7", NULL, "Insert a " REST7 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE7, G_CALLBACK (InsertRest7)},

{
"Set7", NULL, "Set Duration to " NOTE7 "", NULL, "Sets prevailing duration to " NOTE7 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur7)},

#define NOTE8 "\xF0\x9D\x85\xA5"
#define REST8 "\xF0\x9D\x85\x83"
{
"8", "NULL", NOTE8, NULL, "Inserts a note at the cursor with duration " NOTE8 ", or \n(if appending) appends this duration\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.", G_CALLBACK (Dur8)},

{
"Change8", "NULL", NOTE8, NULL, "Change current note to a " NOTE8, G_CALLBACK (ChangeDur8)},

{
"Insert8", NULL, "Insert a " NOTE8 "", NULL, "Inserts a " NOTE8 " at cursor position\nSets prevailing duration to " NOTE8, G_CALLBACK (InsertDur8)},

{
"InsertRest8", NULL, "Insert a " REST8 "rest", NULL, "Inserts a rest at cursor position\nSets prevailing duration to " NOTE8, G_CALLBACK (InsertRest8)},

{
"Set8", NULL, "Set Duration to " NOTE8 "", NULL, "Sets prevailing duration to " NOTE8 " (subsequent notes entered will have this duration)", G_CALLBACK (SetDur8)},

{
"InsertA", NULL, "Insert A", NULL, "Inserts note A before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (InsertA)},

{
"AddNoteA", NULL, "Insert A After", NULL, "Inserts note A after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (AddNoteA)},

{
"AddA", NULL, "Add A to Chord", NULL, "Adds note A to chord at cursor\nCursor determines which octave", G_CALLBACK (AddA)},

{
"ChangeToA", NULL, "Change current note to A", NULL, "Changes current note to the A nearest cursor or (if no current note) inserts the note A\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (ChangeToA)},

{
"MoveToA", NULL, "Move cursor to step A", NULL, "Moves the cursor to the A nearest cursor\nCurrent cursor position determines which octave.", G_CALLBACK (MoveToA)},

{
"InsertB", NULL, "Insert B", NULL, "Inserts note B before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (InsertB)},

{
"AddNoteB", NULL, "Insert B After", NULL, "Inserts note B after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (AddNoteB)},

{
"AddB", NULL, "Add B to Chord", NULL, "Adds note B to chord at cursor\nCursor determines which octave", G_CALLBACK (AddB)},

{
"ChangeToB", NULL, "Change current note to B", NULL, "Changes current note to the B nearest cursor or (if no current note) inserts the note B\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (ChangeToB)},

{
"MoveToB", NULL, "Move cursor to step B", NULL, "Moves the cursor to the B nearest cursor\nCurrent cursor position determines which octave.", G_CALLBACK (MoveToB)},

{
"InsertC", NULL, "Insert C", NULL, "Inserts note C before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (InsertC)},

{
"AddNoteC", NULL, "Insert C After", NULL, "Inserts note C after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (AddNoteC)},

{
"AddC", NULL, "Add C to Chord", NULL, "Adds note C to chord at cursor\nCursor determines which octave", G_CALLBACK (AddC)},

{
"ChangeToC", NULL, "Change current note to C", NULL, "Changes current note to the C nearest cursor or (if no current note) inserts the note C\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (ChangeToC)},

{
"MoveToC", NULL, "Move cursor to step C", NULL, "Moves the cursor to the C nearest cursor\nCurrent cursor position determines which octave.", G_CALLBACK (MoveToC)},

{
"InsertD", NULL, "Insert D", NULL, "Inserts note D before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (InsertD)},

{
"AddNoteD", NULL, "Insert D After", NULL, "Inserts note D after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (AddNoteD)},

{
"AddD", NULL, "Add D to Chord", NULL, "Adds note D to chord at cursor\nCursor determines which octave", G_CALLBACK (AddD)},

{
"ChangeToD", NULL, "Change current note to D", NULL, "Changes current note to the D nearest cursor or (if no current note) inserts the note D\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (ChangeToD)},

{
"MoveToD", NULL, "Move cursor to step D", NULL, "Moves the cursor to the D nearest cursor\nCurrent cursor position determines which octave.", G_CALLBACK (MoveToD)},

{
"InsertE", NULL, "Insert E", NULL, "Inserts note E before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (InsertE)},

{
"AddNoteE", NULL, "Insert E After", NULL, "Inserts note E after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (AddNoteE)},

{
"AddE", NULL, "Add E to Chord", NULL, "Adds note E to chord at cursor\nCursor determines which octave", G_CALLBACK (AddE)},

{
"ChangeToE", NULL, "Change current note to E", NULL, "Changes current note to the E nearest cursor or (if no current note) inserts the note E\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (ChangeToE)},

{
"MoveToE", NULL, "Move cursor to step E", NULL, "Moves the cursor to the E nearest cursor\nCurrent cursor position determines which octave.", G_CALLBACK (MoveToE)},

{
"InsertF", NULL, "Insert F", NULL, "Inserts note F before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (InsertF)},

{
"AddNoteF", NULL, "Insert F After", NULL, "Inserts note F after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (AddNoteF)},

{
"AddF", NULL, "Add F to Chord", NULL, "Adds note F to chord at cursor\nCursor determines which octave", G_CALLBACK (AddF)},

{
"ChangeToF", NULL, "Change current note to F", NULL, "Changes current note to the F nearest cursor or (if no current note) inserts the note F\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (ChangeToF)},

{
"MoveToF", NULL, "Move cursor to step F", NULL, "Moves the cursor to the F nearest cursor\nCurrent cursor position determines which octave.", G_CALLBACK (MoveToF)},

{
"InsertG", NULL, "Insert G", NULL, "Inserts note G before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (InsertG)},

{
"AddNoteG", NULL, "Insert G After", NULL, "Inserts note G after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (AddNoteG)},

{
"AddG", NULL, "Add G to Chord", NULL, "Adds note G to chord at cursor\nCursor determines which octave", G_CALLBACK (AddG)},

{
"ChangeToG", NULL, "Change current note to G", NULL, "Changes current note to the G nearest cursor or (if no current note) inserts the note G\nCursor determines which octave\nNote is inserted in the prevailing rhythm", G_CALLBACK (ChangeToG)},

{
"MoveToG", NULL, "Move cursor to step G", NULL, "Moves the cursor to the G nearest cursor\nCurrent cursor position determines which octave.", G_CALLBACK (MoveToG)},