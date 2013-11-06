register_command("CursorLeft", _("Cursor Left"), _("Moves the cursor one object left, altering the selection if any"), cursorleft);
register_command("MoveCursorLeft", _("Move Cursor Left"), _("Moves the cursor one object left, without altering the selection"), movecursorleft);
register_command("CursorDown", _("Cursor Down"), _("Moves the cursor one scale step down"), cursordown);
register_command("CursorUp", _("Cursor Up"), _("Moves the cursor one scale step up"), cursorup);
register_command("CursorRight", _("Cursor Right"), _("Moves the cursor one object right, altering the selection if any"), cursorright);
register_command("MoveCursorRight", _("Move Cursor Right"), _("Moves the cursor one object right, without altering the selection"), movecursorright);
register_command("GoToMark", _("To Mark"), _("Moves the cursor to the Mark without altering the selection"), goto_mark);
register_command("SwapPointAndMark", _("Swap Ends of Selection"), _("Swaps the active end of the selection"), swap_point_and_mark);
register_command("GoToSelectionStart", _("To Selection Start"), _("Moves the cursor to the first object in the selection without altering the selection. returns #f if no selection"), goto_selection_start);
register_command("PushPosition", _("Push Position"), _("Pushes the current cursor position onto a stack"), PushPosition);
register_command("PopPosition", _("Pop Position"), _("Pops a position from the stack of cursor positions, moving the cursor there"), PopPosition);
register_command("PopPushPosition", _("Pop and Push Position"), _("Pops a position from the stack of cursor positions, pushes the current position, then moves the cursor to the popped position"), PopPushPosition);
register_command("ToggleReduceToDrawingArea", _("Hide/Show Menus"), _("Hides/Shows menus, panes etc. The ones shown are those checked in the view menu."), ToggleReduceToDrawingArea);
register_command("StaffUp", _("Staff Up"), _("Moves the cursor to the staff above, extending selection if any"), staffup);
register_command("StaffDown", _("Staff Down"), _("Moves the cursor to the staff below, extending selection if any"), staffdown);
register_command("MoveToStaffUp", _("Move to Staff Up"), _("Moves the cursor to the staff above without altering selection. On the top staff it adds space above the staffs."), movetostaffup);
register_command("MoveToStaffDown", _("Move to Staff Down"), _("Moves the cursor to the staff below  without altering selection"), movetostaffdown);
register_command("MeasureLeft", _("Measure Left"), _("Moves the cursor to the first object in the next measure, extending selection if any"), measureleft);
register_command("MeasureRight", _("Measure Right"), _("Moves the cursor to the first object in the previous measure, extending selection if any"), measureright);
register_command("MoveToMeasureLeft", _("Move to Measure Left"), _("Moves the cursor to the first object in the next measure leaving selection, if any, unchanged"), movetomeasureleft);
register_command("MoveToMeasureRight", _("Move to Measure Right"), _("Moves the cursor to the first object in the previous measure leaving selection, if any, unchanged"), movetomeasureright);
register_command("A", _("Change/Append A"), _("Changes the note the cursor is on to the note A.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), go_to_A_key);
register_command("B", _("Change/Append B"), _("Changes the note the cursor is on to the note B.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), go_to_B_key);
register_command("C", _("Change/Append C"), _("Changes the note the cursor is on to the note C.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), go_to_C_key);
register_command("D", _("Change/Append D"), _("Changes the note the cursor is on to the note D.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), go_to_D_key);
register_command("E", _("Change/Append E"), _("Changes the note the cursor is on to the note E.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), go_to_E_key);
register_command("F", _("Change/Append F"), _("Changes the note the cursor is on to the note F.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), go_to_F_key);
register_command("G", _("Change/Append G"), _("Changes the note the cursor is on to the note G.\nIf the cursor is in the appending position, appends a note using the prevailing duration."), go_to_G_key);
register_command("OctaveUp", _("Octave Up"), _("Changes the note at the cursor to an octave higher"), octave_up_key);
register_command("OctaveDown", _("Octave Down"), _("Changes the note at the cursor to an octave lower"), octave_down_key);
register_command("WholeNote", _("WholeNote"), _("Insert \xF0\x9D\x85\x9D"), insert_chord_0key);
register_command("HalfNote", _("HalfNote"), _("Insert \xF0\x9D\x85\x9E"), insert_chord_1key);
register_command("QuarterNote", _("QuarterNote"), _("Insert \xF0\x9D\x85\x9F"), insert_chord_2key);
register_command("EighthNote", _("EighthNote"), _("Insert \xF0\x9D\x85\xA0"), insert_chord_3key);
register_command("SixteenthNote", _("SixteenthNote"), _("Insert \xF0\x9D\x85\xA1"), insert_chord_4key);
register_command("ThirtysecondNote", _("ThirtysecondNote"), _("Insert \xF0\x9D\x85\xA2"), insert_chord_5key);
register_command("SixtyfourthNote", _("SixtyfourthNote"), _("Insert \xF0\x9D\x85\xA3"), insert_chord_6key);
register_command("OneHundredTwentyEighthNote", _("OneHundredTwentyEighthNote"), _("Insert \xF0\x9D\x85\xA4"), insert_chord_7key);
register_command("TwoHundredFiftySixthNote", _("TwoHundredFiftySixthNote"), _("Insert \xF0\x9D\x85\xA5"), insert_chord_8key);
register_command("InsertWholeRest", _("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBB</span>"), _("Insert \xF0\x9D\x84\xBB rest"), insert_rest_0key);
register_command("InsertHalfRest", _("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBC</span>"), _("Insert \xF0\x9D\x84\xBC rest"), insert_rest_1key);
register_command("InsertQuarterRest", _("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBD</span>"), _("Insert \xF0\x9D\x84\xBD rest"), insert_rest_2key);
register_command("InsertEighthRest", _("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBE</span>"), _("Insert \xF0\x9D\x84\xBE rest"), insert_rest_3key);
register_command("InsertSixteenthRest", _("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x84\xBF</span>"), _("Insert \xF0\x9D\x84\xBF rest"), insert_rest_4key);
register_command("InsertThirtysecondRest", _("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x85\x80</span>"), _("Insert \xF0\x9D\x85\x80 rest"), insert_rest_5key);
register_command("InsertSixtyfourthRest", _("Insert a <span font_desc=\"Denemo\">\xF0\x9D\x85\x81</span>"), _("Insert \xF0\x9D\x85\x81 rest"), insert_rest_6key);
register_command("InsertBlankWholeNote", _("Insert a \xF0\x9D\x84\xBB Spacer"), _("Insert a non-printing \xF0\x9D\x84\xBB rest"), insert_blankchord_0key);
register_command("InsertBlankHalfNote", _("Insert a \xF0\x9D\x84\xBC Spacer"), _("Insert a non-printing \xF0\x9D\x84\xBC rest"), insert_blankchord_1key);
register_command("InsertBlankQuarterNote", _("Insert a \xF0\x9D\x84\xBD Spacer"), _("Insert a non-printing \xF0\x9D\x84\xBD rest"), insert_blankchord_2key);
register_command("InsertBlankEighthNote", _("Insert a \xF0\x9D\x84\xBE Spacer"), _("Insert a non-printing \xF0\x9D\x84\xBE rest"), insert_blankchord_3key);
register_command("InsertBlankSixteenthNote", _("Insert a \xF0\x9D\x84\xBF Spacer"), _("Insert a non-printing \xF0\x9D\x84\xBF rest"), insert_blankchord_4key);
register_command("InsertBlankThirtysecondNote", _("Insert a \xF0\x9D\x85\x80 Spacer"), _("Insert a non-printing \xF0\x9D\x85\x80 rest"), insert_blankchord_5key);
register_command("InsertBlankSixtyfourthNote", _("Insert a \xF0\x9D\x85\x81 Spacer"), _("Insert a non-printing \xF0\x9D\x85\x81 rest"), insert_blankchord_6key);
register_command("InsertBlankOneHundredTwentyEighthNote", _("Insert a \xF0\x9D\x85\x82 Spacer"), _("Insert a non-printing \xF0\x9D\x85\x82 rest"), insert_blankchord_7key);
register_command("InsertBlankTwoHundredFiftySixthNote", _("Insert a \xF0\x9D\x85\x83 Spacer"), _("Insert a non-printing \xF0\x9D\x85\x83 rest"), insert_blankchord_8key);
register_command("ToggleRestMode", _("Toggle Rest Mode"), _("No Tooltip yet"), rest_toggle_key);
register_command("ToggleBlankMode", _("Toggle Blank Mode"), _("No Tooltip yet"), toggle_blank);
register_command("InsertDuplet", _("Insert Duplet"), _("No Tooltip yet"), insert_duplet);
register_command("InsertTriplet", _("Insert Triplet"), _("Inserts a Start Triplet object and an End Tuplet object and places the cursor between these two"), insert_triplet);
register_command("StartTriplet", _("Start Triplet"), _("Inserts an Start Triplet object, which makes the notes following take 2/3 of their written duration. Later in this measure there should be an End Tuplet object."), start_triplet);
register_command("EndTuplet", _("End Tuplet"), _("Inserts an End Tuplet object, which terminates a tuplet started earlier in this measure."), end_tuplet);
register_command("InsertQuadtuplet", _("Insert Quadruplet"), _("No Tooltip yet"), insert_quadtuplet);
register_command("InsertQuintuplet", _("Insert Quintuplet"), _("No Tooltip yet"), insert_quintuplet);
register_command("InsertSextuplet", _("Insert Sextuplet"), _("No Tooltip yet"), insert_sextuplet);
register_command("InsertSeptuplet", _("Insert Septuplet"), _("No Tooltip yet"), insert_septuplet);
register_command("AddNoteToChord", _("Add note"), _("Add a note to the current chord\nThe cursor position determines which note to add"), add_tone_key);
register_command("RemoveNoteFromChord", _("Remove note"), _("Remove a note from the current chord, based on the cursor position"), remove_tone_key);
register_command("Sharpen", _("Sharpen"), _("Sharpen the note at the cursor"), sharpen_key);
register_command("Flatten", _("Flatten"), _("Flatten the note at the cursor"), flatten_key);
register_command("PendingSharpen", _("Sharpen Next Note"), _("Increases the sharpness of the next entered note. The status bar shows the current state."), pending_sharpen);
register_command("PendingFlatten", _("Flatten Next Note"), _("Increases the flatness of the next entered note. The status bar shows the current state."), pending_flatten);
register_command("StemUp", _("StemUp"), _("Alters a StemNeutral object to stem up."), stem_up);
register_command("StemDown", _("StemDown"), _("Alters a StemNeutral object to stem down."), stem_down);
register_command("AddDot", _("Add Dot"), _("Lengthen the chord, note or rest at the cursor by dotting it."), add_dot_key);
register_command("RemoveDot", _("Remove Dot"), _("Reduce the dotting of the chord note or rest at the cursor."), remove_dot_key);
register_command("InsertTiedNote", _("Tied note"), _("Inserts a duplicate of the current note, tied"), tie_notes_key);
register_command("ToggleTie", _("Toggle Tie"), _("Ties/unties the note at the cursor"), toggle_tie);
register_command("DeleteObject", _("Delete Object"), _("Delete the object at the cursor"), deleteobject);
register_command("DeletePreviousObject", _("Delete Previous Object"), _("Delete to the left of the cursor."), deletepreviousobject);
register_command("InsertMeasure", _("Insert Measure Before"), _("Insert a blank measure before the current one (in all staffs)"), insert_measure_key);
register_command("AddMeasure", _("Insert Measure After"), _("Insert a blank measure after the current one (in all staffs)"), addmeasureafter);
register_command("InsertMeasureBefore", _("Staff Insert Measure Before"), _("Insert a blank measure before the current one (in current staff)"), insertmeasurebefore);
register_command("InsertMeasureAfter", _("Staff Insert Measure After"), _("Insert a blank measure in current staff after the current measure"), insertmeasureafter);
register_command("AppendMeasure", _("Staff Append Measure"), _("Append an empty measure at the end of the current staff"), append_measure_key);
register_command("DeleteMeasure", _("Staff Delete Measure"), _("Delete the current measure in this staff, leaving the staff short"), deletemeasure);
register_command("DeleteMeasureAllStaffs", _("Delete Measure All Staffs"), _("Delete the current measure in all staffs"), deletemeasureallstaffs);
register_command("ShrinkMeasures", _("Shrink Measure"), _("No Tooltip yet"), adjust_measure_less_width_key);
register_command("WidenMeasures", _("Widen Measures"), _("No Tooltip yet"), adjust_measure_more_width_key);
register_command("ShorterStaffs", _("Shorter Staffs"), _("No Tooltip yet"), adjust_staff_less_height_key);
register_command("TallerStaffs", _("Taller Staffs"), _("No Tooltip yet"), adjust_staff_more_height_key);
register_command("InsertTrebleClef", _("New Treble Clef"), _("No Tooltip yet"), newcleftreble);
register_command("InsertBassClef", _("New Bass Clef"), _("No Tooltip yet"), newclefbass);
register_command("Insertg8clef", _("New G8 Clef"), _("No Tooltip yet"), newclefg8);
register_command("InsertAltoClef", _("New Alto Clef"), _("No Tooltip yet"), newclefalto);
register_command("InsertTenorClef", _("New Tenor Clef"), _("No Tooltip yet"), newcleftenor);
register_command("InsertSopranoClef", _("New Soprano Clef"), _("No Tooltip yet"), newclefsoprano);
register_command("SetInitialTrebleClef", _("Set Treble Clef"), _("No Tooltip yet"), setcleftreble);
register_command("SetInitialBassClef", _("Set Bass Clef"), _("No Tooltip yet"), setclefbass);
register_command("SetInitialg8clef", _("Set G8 Clef"), _("No Tooltip yet"), setclefg8);
register_command("SetInitialAltoClef", _("Set Alto Clef"), _("No Tooltip yet"), setclefalto);
register_command("SetInitialTenorClef", _("Set Tenor Clef"), _("No Tooltip yet"), setcleftenor);
register_command("SetInitialSopranoClef", _("Set Soprano Clef"), _("No Tooltip yet"), setclefsoprano);
register_command("Insert22Time", _("Insert 2/2 Time"), _("No Tooltip yet"), newtimesig22);
register_command("Insert32Time", _("Insert 3/2 Time"), _("No Tooltip yet"), newtimesig32);
register_command("Insert42Time", _("Insert 4/2 Time"), _("No Tooltip yet"), newtimesig42);
register_command("Insert44Time", _("Insert 4/4 Time"), _("No Tooltip yet"), newtimesig44);
register_command("Insert34Time", _("Insert 3/4 Time"), _("No Tooltip yet"), newtimesig34);
register_command("Insert24Time", _("Insert 2/4 Time"), _("No Tooltip yet"), newtimesig24);
register_command("Insert64Time", _("Insert 6/4 Time"), _("No Tooltip yet"), newtimesig64);
register_command("Insert38Time", _("Insert 3/8 Time"), _("No Tooltip yet"), newtimesig38);
register_command("Insert68Time", _("Insert 6/8 Time"), _("No Tooltip yet"), newtimesig68);
register_command("Insert128Time", _("Insert 12/8 Time"), _("No Tooltip yet"), newtimesig128);
register_command("Insert98Time", _("Insert 9/8 Time"), _("No Tooltip yet"), newtimesig98);
register_command("Set22Time", _("Set 2/2 Time"), _("No Tooltip yet"), settimesig22);
register_command("Set32Time", _("Set 3/2 Time"), _("No Tooltip yet"), settimesig32);
register_command("Set42Time", _("Set 4/2 Time"), _("No Tooltip yet"), settimesig42);
register_command("Set44Time", _("Set 4/4 Time"), _("No Tooltip yet"), settimesig44);
register_command("Set34Time", _("Set 3/4 Time"), _("No Tooltip yet"), settimesig34);
register_command("Set24Time", _("Set 2/4 Time"), _("No Tooltip yet"), settimesig24);
register_command("Set64Time", _("Set 6/4 Time"), _("No Tooltip yet"), settimesig64);
register_command("Set38Time", _("Set 3/8 Time"), _("No Tooltip yet"), settimesig38);
register_command("Set68Time", _("Set 6/8 Time"), _("No Tooltip yet"), settimesig68);
register_command("Set128Time", _("Set 12/8 Time"), _("No Tooltip yet"), settimesig128);
register_command("Set98Time", _("Set 9/8 Time"), _("No Tooltip yet"), settimesig98);
register_command("InsertCmaj", _("Insert Cmaj"), _("No Tooltip yet"), newkeysigcmaj);
register_command("InsertGmaj", _("Insert Gmaj"), _("No Tooltip yet"), newkeysiggmaj);
register_command("InsertDmaj", _("Insert Dmaj"), _("No Tooltip yet"), newkeysigdmaj);
register_command("InsertAmaj", _("Insert Amaj"), _("No Tooltip yet"), newkeysigamaj);
register_command("InsertEmaj", _("Insert Emaj"), _("No Tooltip yet"), newkeysigemaj);
register_command("InsertBmaj", _("Insert Bmaj"), _("No Tooltip yet"), newkeysigbmaj);
register_command("InsertFSharpmaj", _("Insert F# Major"), _("No Tooltip yet"), newkeysigfsharpmaj);
register_command("InsertCSharpmaj", _("Insert C# Major"), _("No Tooltip yet"), newkeysigcsharpmaj);
register_command("InsertFmaj", _("Insert F Major"), _("No Tooltip yet"), newkeysigfmaj);
register_command("InsertBflatmaj", _("Insert Bb Major"), _("No Tooltip yet"), newkeysigbflatmaj);
register_command("InsertEflatmaj", _("Insert Eb Major"), _("No Tooltip yet"), newkeysigeflatmaj);
register_command("InsertAflatmaj", _("Insert Ab Major"), _("No Tooltip yet"), newkeysigaflatmaj);
register_command("InsertDflatmaj", _("Insert Db Major"), _("No Tooltip yet"), newkeysigdflatmaj);
register_command("InsertGflatmaj", _("Insert Gb Major"), _("No Tooltip yet"), newkeysiggflatmaj);
register_command("InsertCflatmaj", _("Insert Cb Major"), _("No Tooltip yet"), newkeysigcflatmaj);
register_command("InsertAmin", _("Insert A Minor"), _("No Tooltip yet"), newkeysigamin);
register_command("InsertEmin", _("Insert E Minor"), _("No Tooltip yet"), newkeysigemin);
register_command("InsertBmin", _("Insert B Minor"), _("No Tooltip yet"), newkeysigbmin);
register_command("InsertFSharpmin", _("Insert F# Minor"), _("No Tooltip yet"), newkeysigfsharpmin);
register_command("InsertCSharpmin", _("Insert C# Minor"), _("No Tooltip yet"), newkeysigcsharpmin);
register_command("InsertGSharpmin", _("Insert G# Minor"), _("No Tooltip yet"), newkeysiggsharpmin);
register_command("InsertDSharpmin", _("Insert D# Minor"), _("No Tooltip yet"), newkeysigdsharpmin);
register_command("InsertASharpmin", _("Insert A# Minor"), _("No Tooltip yet"), newkeysigasharpmin);
register_command("InsertDmin", _("Insert D Minor"), _("No Tooltip yet"), newkeysigdmin);
register_command("InsertGmin", _("Insert G Minor"), _("No Tooltip yet"), newkeysiggmin);
register_command("InsertCmin", _("Insert C Minor"), _("No Tooltip yet"), newkeysigcmin);
register_command("InsertFmin", _("Insert F Minor"), _("No Tooltip yet"), newkeysigfmin);
register_command("InsertBflatmin", _("Insert Bb Minor"), _("No Tooltip yet"), newkeysigbflatmin);
register_command("InsertEflatmin", _("Insert Eb Minor"), _("No Tooltip yet"), newkeysigeflatmin);
register_command("InsertAflatmin", _("Insert Ab Minor"), _("No Tooltip yet"), newkeysigaflatmin);
register_command("SetInitialCmaj", _("Set Initial Keysig to C Major"), _("No Tooltip yet"), setkeysigcmaj);
register_command("SetInitialGmaj", _("Set Initial Keysig to G Major"), _("No Tooltip yet"), setkeysiggmaj);
register_command("SetInitialDmaj", _("Set D Major as Initial Keysig"), _("No Tooltip yet"), setkeysigdmaj);
register_command("SetInitialAmaj", _("Set A Major as Initial Keysig"), _("No Tooltip yet"), setkeysigamaj);
register_command("SetInitialEmaj", _("Set E Major as Initial Keysig"), _("No Tooltip yet"), setkeysigemaj);
register_command("SetInitialBmaj", _("Set B Major as Initial Keysig"), _("No Tooltip yet"), setkeysigbmaj);
register_command("SetInitialFSharpmaj", _("Set F# Major as Initial Keysig"), _("No Tooltip yet"), setkeysigfsharpmaj);
register_command("SetInitialCSharpmaj", _("Set C# Major as Initial Keysig"), _("No Tooltip yet"), setkeysigcsharpmaj);
register_command("SetInitialFmaj", _("Set F Major as Initial Keysig"), _("No Tooltip yet"), setkeysigfmaj);
register_command("SetInitialBflatmaj", _("Set Bb Major as Initial Keysig"), _("No Tooltip yet"), setkeysigbflatmaj);
register_command("SetInitialEflatmaj", _("Set Eb Major as Initial Keysig"), _("No Tooltip yet"), setkeysigeflatmaj);
register_command("SetInitialAflatmaj", _("Set Ab Major as Initial Keysig"), _("No Tooltip yet"), setkeysigaflatmaj);
register_command("SetInitialDflatmaj", _("Set Db Major as Initial Keysig"), _("No Tooltip yet"), setkeysigdflatmaj);
register_command("SetInitialGflatmaj", _("Set Gb Major as Initial Keysig"), _("No Tooltip yet"), setkeysiggflatmaj);
register_command("SetInitialCflatmaj", _("Set Cb Major as Initial Keysig"), _("No Tooltip yet"), setkeysigcflatmaj);
register_command("SetInitialAmin", _("Set A Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigamin);
register_command("SetInitialEmin", _("Set E Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigemin);
register_command("SetInitialBmin", _("Set B Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigbmin);
register_command("SetInitialFSharpmin", _("Set F# Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigfsharpmin);
register_command("SetInitialCSharpmin", _("Set C# Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigcsharpmin);
register_command("SetInitialGSharpmin", _("Set G# Minor as Initial Keysig"), _("No Tooltip yet"), setkeysiggsharpmin);
register_command("SetInitialDSharpmin", _("Set D# Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigdsharpmin);
register_command("SetInitialASharpmin", _("Set A# Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigasharpmin);
register_command("SetInitialDmin", _("Set D Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigdmin);
register_command("SetInitialGmin", _("Set G Minor as Initial Keysig"), _("No Tooltip yet"), setkeysiggmin);
register_command("SetInitialCmin", _("Set C Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigcmin);
register_command("SetInitialFmin", _("Set F Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigfmin);
register_command("SetInitialBflatmin", _("Set Bb Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigbflatmin);
register_command("SetInitialEflatmin", _("Set Eb Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigeflatmin);
register_command("SetInitialAflatmin", _("Set Ab Minor as Initial Keysig"), _("No Tooltip yet"), setkeysigaflatmin);
register_command("SetMark", _("Set Mark"), _("Sets the start point for a selection,\nthe end point of the selection is unaltered"), set_mark);
register_command("UnsetMark", _("Unset Mark"), _("Gets rid of the selection."), unset_mark);
register_command("SetPoint", _("Set Point"), _("Extends the selection to the current cursor position"), set_point);
register_command("ToggleBeginSlur", _("Begin Slur"), _("Insert/delete begin slur on this note"), toggle_begin_slur);
register_command("ToggleEndSlur", _("End Slur"), _("Insert/delete end slur on this note"), toggle_end_slur);
register_command("ToggleStartCrescendo", _("Start Crescendo (Off/On)"), _("Marks/Unmarks the chord or note at the cursor as the start of a crescendo."), toggle_start_crescendo);
register_command("ToggleEndCrescendo", _("End Crescendo (Off/On)"), _("Marks/Unmarks the chord or note at the cursor as the end of a crescendo."), toggle_end_crescendo);
register_command("ToggleStartDiminuendo", _("Start Diminuendo (Off/On)"), _("Marks/Unmarks the chord or note at the cursor as the start of a diminuendo."), toggle_start_diminuendo);
register_command("ToggleEndDiminuendo", _("End Diminuendo (Off/On)"), _("Marks/Unmarks the chord or note at the cursor as the end of a diminuendo."), toggle_end_diminuendo);
register_command("ToggleGrace", _("Grace Note Off/On"), _("Makes the note at the cursor an appogiatura grace note, if it is one, makes it normal"), toggle_grace);
register_command("ToggleAcciaccatura", _("Acciaccatura Off/On"), _("Makes the note at the cursor an acciaccatura grace note, if it is one, makes it normal"), toggle_acciaccatura);
register_command("ForceCaution", _("Force Cautionary Accidental"), _("Give a cautionary accidental to the note at the cursor"), force_cautionary);
register_command("ChangePitch", _("Change Pitch"), _("Changes the pitch of the note at the cursor to the cursor height"), change_pitch);
register_command("InsertRhythm", _("Insert Snippet"), _("No Tooltip yet"), insert_rhythm_pattern);
register_command("NextRhythm", _("Next Snippet"), _("Make next snippet\nthe current snippet.\nNotes entered will follow the rhythmic pattern of this snippet"), nextrhythm);
register_command("AppendMeasureAllStaffs", _("Append Measure All Staffs"), _("Appends a blank measure to every staff in this movement"), append_measure_score);
register_command("ExecuteScheme", _("Execute Scheme"), _("Execute the scheme code from the scripting window"), execute_scheme);
register_command("SharpenEnharmonicSet", _("Shift Accidentals Sharpwise"), _("Shifts the set of accidentals one step sharper"), set_sharper);
register_command("FlattenEnharmonicSet", _("Shift Accidentals Flatwise"), _("Shifts the set of accidentals one step flatter"), set_flatter);
register_command("New", _("Empty Score"), _("Start a new musical score"), file_newwrapper);
register_command("NewScore", _("New"), _("Start a new musical score for a named instrument/voice."), new_score_cb);
register_command("Open", _("Open"), _("Open a file containing a music score for editing"), file_open_with_check);
register_command("ImportLilypond", _("Import Lilypond"), _("Import a Lilypond file"), file_import_lilypond_with_check);
register_command("ImportMidi", _("Import Midi"), _("Import a Midi file"), file_import_midi_with_check);
register_command("ImportMusicXml", _("Import MusicXml"), _("Import a MusicXml file"), file_import_musicxml_with_check);
register_command("AddStaffs", _("Add Staffs"), _("Add staffs from a Denemo file"), file_add_staffs);
register_command("AddMovements", _("Add Movement"), _("Add movements from a Denemo file"), file_add_movements);
register_command("MovementProps", _("Change Properties"), _("Change properties of this movement"), movement_props_dialog);
register_command("OpenNewWindow", _("Open In New"), _("Open a file containing a music score for editing in a separate working area (tab)"), openinnew);
register_command("Save", _("Save"), _("Save the score. The score is saved to disk in XML format."), file_savewrapper);
register_command("SaveAs", _("Save As"), _("Save the score under a new name"), file_saveaswrapper);
register_command("SaveCopy", _("Create Copy"), _("Save a copy of the score"), file_copy_save);
register_command("OpenTemplate", _("Open Template"), _("Start a new score from a built-in template file"), system_template_open_with_check);
register_command("OpenExample", _("Open Example"), _("Start a new score from a built-in example"), system_example_open_with_check);
register_command("OpenMyTemplate", _("Open Custom Template"), _("Start a new score from one of your own template files"), local_template_open_with_check);
register_command("SaveTemplate", _("Save Template"), _("Save the score as a template for re-use as a starting point for new scores"), template_save);
register_command("NewWindow", _("New Tab"), _("Create working area (tab with an empty score in it)"), newview);
register_command("InsertMovementBefore", _("Insert Movement Before"), _("Insert a new movement before the current one"), insert_movement_before);
register_command("InsertMovementAfter", _("Insert Movement After"), _("Insert a new movement after the current one"), insert_movement_after);
register_command("NewMovement", _("New Movement"), _("Create a new movement, usign any default template"), append_new_movement);
register_command("SaveParts", _("Save Parts"), _("Save Parts: each staff becomes a file in lilypond format"), file_savepartswrapper);
register_command("ExportMUDELA", _("Export Lilypond"), _("Export the score as a lilypond file"), export_mudela_action);
register_command("ExportPDF", _("Export PDF"), _("Export the score as a PDF document file"), export_pdf_action);
register_command("ExportPNG", _("Export Score as PNG"), _("Export the score as a PNG image file"), export_png_action);
register_command("ExportMIDI", _("Export MIDI"), _("Export the score as a MIDI file"), export_midi_action);
register_command("PrintView", _("Print Preview"), _("Typesets the score\nIf you have a score layout selected it will use that\notherwise all movements staffs and lyrics are typeset by default.\nBe patient! It takes time to create a beautifully laid out score.\nOnce complete you can view and then send to your printer or to a file as a .pdf document."), show_print_view);
register_command("PrintSelection", _("Print Selection"), _("Displays selected music from score in your pdf viewer"), printselection_cb);
register_command("PrintExcerptPreview", _("Export Selection as PNG"), _("Displays a musical excerpt in your image viewer"), printexcerptpreview_cb);
register_command("PrintMovement", _("Print Movement"), _("Typesets the current movement and opens a print dialog"), printmovement_cb);
register_command("Print", _("Print"), _("Typesets the score using LilyPond and opens a print dialog"), printall_cb);
register_command("PrintPart", _("Print Part"), _("Typesets the current part (the one containing the cursor)."), printpart_cb);
register_command("Close", _("Close Score"), _("Close the current score. Other scores (tabs) will stay open"), close_gui_with_check);
register_command("Quit", _("Quit"), _("Quit the Denemo program - closes tabs one at a time."), closewrapper);
register_command("Undo", _("Undo"), _("Undoes one (more) step of your edits to the current score."), undowrapper);
register_command("Redo", _("Redo"), _("Redoes the next of the steps you have Undone"), redowrapper);
register_command("Copy", _("Copy"), _("Copy the music selected to the Denemo clipboard"), copywrapper);
register_command("Cut", _("Cut"), _("Cut the music selected to the Denemo clipboard"), cutwrapper);
register_command("Paste", _("Paste"), _("Paste the Denemo clipboard into the score where the cursor is positioned"), pastewrapper);
register_command("PasteClipboard", _("Paste LilyPond notes"), _("Paste LilyPond notes from the text clipboard\nThis will import music written as LilyPond syntax\nYou open the LilyPond file in a texteditor, copy the stretch of notes (control-c command in your texteditor usually) and then use this command."), paste_clipboard);
register_command("ScoreProperties", _("Score Properties"), _("Change built-in properties of the current score. This will start up a dialog window"), score_properties_dialog);
register_command("SaveSelection", _("Save Selection"), _("Save the selected music. Not sure if this is working"), saveselwrapper);
register_command("Preferences", _("Change Preferences"), _("Set and save your preferences for how Denemo operates on startup.\nAdvanced users can edit .denemo-XXXX/denemorc for missing ones"), preferences_change);
register_command("SaveAccels", _("Save Command Set"), _("Save the current commands and keyboard shortcuts as the default"), save_default_keymap_file_wrapper);
register_command("CommandManagement", _("Manage Command Set"), _("View help, change and save keyboard shortcuts"), configure_keyboard_dialog);
register_command("SwapStaffs", _("Swap Staffs"), _("Swap this staff with the one higher up.\nBe aware that if you have inserted directives to move a voice to another staff\nthese may need re-making."), swapstaffs);
register_command("SplitVoices", _("Split Voices"), _("Split off the next voice as a separate staff"), splitstaffs);
register_command("JoinVoices", _("Join Voices"), _("Merge this staff as a voice on the previous staff"), joinstaffs);
register_command("SwapMovements", _("Swap Movements"), _("Swap this movement with the one before"), swapmovements);
register_command("VoiceUp", _("Voice Up"), _("Go to the higher numbered voice on staff, extending selection if any"), voiceup);
register_command("VoiceDown", _("Voice Down"), _("Go to the lower numbered voice on this staff, extending selection if any"), voicedown);
register_command("MoveToVoiceUp", _("Move to Voice Up"), _("Go to the higher numbered voice on staff without altering selection"), movetovoiceup);
register_command("MoveToVoiceDown", _("Move to Voice Down"), _("Go to the lower numbered voice on this staff without altering selection"), movetovoicedown);
register_command("AddBefore", _("Add Staff Before"), _("Inserts a new staff before the current staff"), newstaffbefore);
register_command("AddAfter", _("Add Staff After"), _("Inserts/Adds a new staff after the current staff"), dnm_newstaffafter);
register_command("AddInitial", _("Add Initial Staff"), _("Inserts a new staff at the top of the score"), newstaffinitial);
register_command("AddLast", _("Add Last Staff"), _("Inserts a new staff at the end of the score"), newstafflast);
register_command("DeleteBefore", _("Delete Staff Before"), _("Deletes the staff before the current staff"), delete_staff_before);
register_command("DeleteStaff", _("Delete Current Staff"), _("Deletes the current staff"), delete_staff_current);
register_command("DeleteAfter", _("Delete Staff After"), _("Deletes the staff after the current staff"), delete_staff_after);
register_command("AddVoice", _("Add Voice"), _("Adds a new voice (part), to the current staff. It is tricky to switch between the voices. Suggest to use merge staffs"), dnm_newstaffvoice);
register_command("StaffProperties", _("Built-in Staff Properties"), _("Change the built-in properties of the current staff"), staff_properties_change_cb);
register_command("InitialClef", _("Initial Clef"), _("Change the initial clef of the current staff"), clef_change_initial);
register_command("InsertClef", _("Clef Change"), _("Insert/Edit a change of clef at the cursor"), clef_change_insert);
register_command("InitialKey", _("Initial Key"), _("Set the initial key signature of the current staff"), key_change_initial);
register_command("InsertKey", _("Key Signature Change"), _("Insert/Edit a key change at the cursor position"), key_change_insert);
register_command("InitialTimeSig", _("Inital Time Signature"), _("Set the initial time signature of the current staff"), timesig_change_initial);
register_command("InsertTimeSig", _("Time Signature Change"), _("Edit/Insert a time signature change for the current measure"), timesig_change_insert);
register_command("ChangeNotehead", _("Set Notehead"), _("Change the type of notehead for the current note"), set_notehead);
register_command("InsertStem", _("Auto Stemming"), _("Inserts a stem neutral object. After this automatic stem directions are active. You can click on this tag and use Sharpen/StemUp etc commands to change stem direction"), stem_directive_insert);
register_command("AddVerse", _("Add Lyric Verse"), _("Add a verse of lyrics"), add_verse);
register_command("DeleteVerse", _("Delete Verse"), _("Deletes current verse of lyrics from current voice"), delete_verse);
register_command("EditFiguredBass", _("Insert/Edit Figured Bass"), _("Add a bass figure to the current note. Use | sign to split the duration of a note so as to have multiple figures on one note. See Lilypond docs for other notation"), figure_insert);
register_command("DeleteFiguredBass", _("Delete Figures"), _("Delete the figured bass on the current staff"), delete_figured_bass);
register_command("HideFiguredBass", _("Hide Figures (Print)"), _("Hide the figured bass on the current staff on printing"), hide_figured_bass);
register_command("ShowFiguredBass", _("Show Figures (Print)"), _("Show the figured bass on the current staff on printing"), show_figured_bass);
register_command("EditChords", _("Edit Chord Symbols"), _("Allows chord symbols to be added to the current note. E.G.cis:dim7 for c-sharp diminished 7th. See Lilypond docs for notation"), fakechord_insert);
register_command("InsertDynamic", _("Insert Dynamics"), _("Inserts a dynamic marking at the cursor position"), insert_dynamic);
register_command("EditObject", _("Edit Object"), _("Edit the object at the cursor."), edit_object);
register_command("EditDirective", _("Edit Directives"), _("Edit any directives attached to chord/note at cursor."), edit_object_directive);
register_command("EditStaffDirective", _("Edit Staff Directives"), _("Edit any directives attached to staff."), edit_staff_directive);
register_command("EditVoiceDirective", _("Edit Voice Directives"), _("Edit any directives attached to voice."), edit_voice_directive);
register_command("EditScoreDirective", _("Edit Score Directives"), _("Edit any directives attached to score."), edit_score_directive);
register_command("EditMovementDirective", _("Edit Movement Directives"), _("Edit any directives attached to movement."), edit_movement_directive);
register_command("EditClefDirective", _("Edit Clef Directives"), _("Edit any directives attached to clef."), edit_clef_directive);
register_command("EditTimesigDirective", _("Edit Time Signature Directives"), _("Edit any directives attached to time signature."), edit_timesig_directive);
register_command("EditKeysigDirective", _("Edit Key Signature Directives"), _("Edit any directives attached to key signature."), edit_keysig_directive);
register_command("DeleteDirective", _("Delete a Directive"), _("Delete a directive attached to chord/note at cursor."), delete_chord_or_note_directive);
register_command("AttachLilyToNote", _("Attach Lilypond to Note"), _("Attach or edit attached LilyPond text to the note at the cursor. This can be used for guitar fingerings, cautionary accidentals and much more. See LilyPond documentation."), note_directive);
register_command("AttachLilyToChord", _("Attach Lilypond to Chord"), _("Attach or edit attached LilyPond text to the chord at the cursor. This can be used for attaching and placing text and much more. See LilyPond documentation."), chord_directive);
register_command("InsertBarline", _("Insert Barline"), _("Inserts specialized barline at the cursor position. Mostly not working"), insert_barline);
register_command("GoToMeasure", _("Go to Measure"), _("Opens a dialog for going to a numbered measure"), tomeasurenum);
register_command("GoToBeginning", _("Go to Beginning"), _("Cursor to start of staff/voice, extending selection if any"), tohome);
register_command("GoToEnd", _("Go to End"), _("Cursor to end of staff/voice, extending selection if any"), toend);
register_command("MoveToBeginning", _("Move to Staff/Voice Beginning"), _("Cursor to start of staff/voice, without extending selection if any"), movetostart);
register_command("MoveToEnd", _("Move to Staff/Voice End"), _("Cursor to end of staff/voice, without extending selection if any"), movetoend);
register_command("NextMovement", _("Next Movement"), _("Go to the next movement"), next_movement);
register_command("PreviousMovement", _("Previous Movement"), _("Go to the previous movement"), prev_movement);
register_command("DeleteMovement", _("Delete Movement"), _("Delete the current movement"), delete_movement);
register_command("Play", _("Play"), _("Playback from start marker to end marker (Set these markers in the playback controls)"), ext_midi_playback);
register_command("Stop", _("Stop"), _("Stop Playback"), stop_midi_playback);
register_command("PlaybackProperties", _("Playback Properties"), _("Allows you to specify properties used in playing back (midi)"), playback_properties_change);
register_command("Help", _("Browse Manual"), _("Opens a browser on the user manual"), browse_manual);
register_command("About", _("About"), _("Gives the version number etc of this program"), about);
register_command("MoreCommands", _("More Commands"), _("Allows choosing standard extra commands/menu items"), morecommands);
register_command("MyCommands", _("My Commands"), _("Allows choosing extra commands/menu items from your own collection of extras"), mycommands);
register_command("FetchCommands", _("Update Commands from Internet"), _("Refreshes the set of commands available from Denemo.org.\nUse More Commands after this has finished"), fetchcommands);
register_command("ToggleEdit", _("Toggle Edit Mode"), _("Toggle between current mode and edit mode"), toggle_edit_mode);
register_command("ToggleRest", _("Toggle Rest Mode"), _("Toggle between note entry and rest entry"), toggle_rest_mode);
register_command("ToggleRhythm", _("Toggle Audible Feedback"), _("Toggle audible feedback on/off"), toggle_rhythm_mode);
register_command("ClearOverlay", _("Clear Overlay"), _("Clear the list of pitches that overlay the notes"), clear_overlay);
register_command("CreateRhythm", _("Create Snippet"), _("Copy selection as music snippet or rhythm pattern for notes to follow as they are entered"), create_rhythm_cb);
register_command("DeleteRhythm", _("Delete Snippet"), _("Delete the selected music snippet/rhythm pattern"), delete_rhythm_cb);
register_command("InsertA", _("Insert A"),_("Inserts note A before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  InsertA);
register_command("AddNoteA", _("Insert A After"),_("Inserts note A after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  AddNoteA);
register_command("AddA", _("Add A"),_("Adds note A to the chord at cursor\nCursor height determines which octave"),  AddA);
register_command("ChangeToA", _("Change to A"),_("Changes note at cursor to nearest note A\nRhythm is unchanged"),  ChangeToA);
register_command("MoveToA", _("Move to A"),_("Moves cursor to nearest note A"),  MoveToA);
register_command("InsertB", _("Insert B"),_("Inserts note B before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  InsertB);
register_command("AddNoteB", _("Insert B After"),_("Inserts note B after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  AddNoteB);
register_command("AddB", _("Add B"),_("Adds note B to the chord at cursor\nCursor height determines which octave"),  AddB);
register_command("ChangeToB", _("Change to B"),_("Changes note at cursor to nearest note B\nRhythm is unchanged"),  ChangeToB);
register_command("MoveToB", _("Move to B"),_("Moves cursor to nearest note B"),  MoveToB);
register_command("InsertC", _("Insert C"),_("Inserts note C before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  InsertC);
register_command("AddNoteC", _("Insert C After"),_("Inserts note C after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  AddNoteC);
register_command("AddC", _("Add C"),_("Adds note C to the chord at cursor\nCursor height determines which octave"),  AddC);
register_command("ChangeToC", _("Change to C"),_("Changes note at cursor to nearest note C\nRhythm is unchanged"),  ChangeToC);
register_command("MoveToC", _("Move to C"),_("Moves cursor to nearest note C"),  MoveToC);
register_command("InsertD", _("Insert D"),_("Inserts note D before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  InsertD);
register_command("AddNoteD", _("Insert D After"),_("Inserts note D after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  AddNoteD);
register_command("AddD", _("Add D"),_("Adds note D to the chord at cursor\nCursor height determines which octave"),  AddD);
register_command("ChangeToD", _("Change to D"),_("Changes note at cursor to nearest note D\nRhythm is unchanged"),  ChangeToD);
register_command("MoveToD", _("Move to D"),_("Moves cursor to nearest note D"),  MoveToD);
register_command("InsertE", _("Insert E"),_("Inserts note E before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  InsertE);
register_command("AddNoteE", _("Insert E After"),_("Inserts note E after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  AddNoteE);
register_command("AddE", _("Add E"),_("Adds note E to the chord at cursor\nCursor height determines which octave"),  AddE);
register_command("ChangeToE", _("Change to E"),_("Changes note at cursor to nearest note E\nRhythm is unchanged"),  ChangeToE);
register_command("MoveToE", _("Move to E"),_("Moves cursor to nearest note E"),  MoveToE);
register_command("InsertF", _("Insert F"),_("Inserts note F before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  InsertF);
register_command("AddNoteF", _("Insert F After"),_("Inserts note F after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  AddNoteF);
register_command("AddF", _("Add F"),_("Adds note F to the chord at cursor\nCursor height determines which octave"),  AddF);
register_command("ChangeToF", _("Change to F"),_("Changes note at cursor to nearest note F\nRhythm is unchanged"),  ChangeToF);
register_command("MoveToF", _("Move to F"),_("Moves cursor to nearest note F"),  MoveToF);
register_command("InsertG", _("Insert G"),_("Inserts note G before note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  InsertG);
register_command("AddNoteG", _("Insert G After"),_("Inserts note G after note at cursor\nCursor determines which octave\nNote is inserted in the prevailing rhythm"),  AddNoteG);
register_command("AddG", _("Add G"),_("Adds note G to the chord at cursor\nCursor height determines which octave"),  AddG);
register_command("ChangeToG", _("Change to G"),_("Changes note at cursor to nearest note G\nRhythm is unchanged"),  ChangeToG);
register_command("MoveToG", _("Move to G"),_("Moves cursor to nearest note G"),  MoveToG);
register_command("0", _("Insert/Append a \xF0\x9D\x85\x9D"), _("When appending, appends a \xF0\x9D\x85\x9D \nWith the cursor on a note inserts a \xF0\x9D\x85\x9D  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur0);
register_command("Change0", _("Change to \xF0\x9D\x85\x9D"), _("Change the current note to a \xF0\x9D\x85\x9D"), ChangeDur0);
register_command("Insert0", _("\xF0\x9D\x85\x9D"), _("Insert a \xF0\x9D\x85\x9D"), InsertDur0);
register_command("InsertRest0",  _("Insert a \xF0\x9D\x84\xBB") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\x9D"), InsertRest0);
register_command("Set0", _("Set Prevailing Duration to \xF0\x9D\x85\x9D"), _("Set the prevailing duration to \xF0\x9D\x85\x9D (subsequent notes entered will have this duration)"), SetDur0);

#undef NOTE0

#undef REST0

#undef NOTECHAR0

#undef RESTCHAR0
register_command("1", _("Insert/Append a \xF0\x9D\x85\x9E"), _("When appending, appends a \xF0\x9D\x85\x9E \nWith the cursor on a note inserts a \xF0\x9D\x85\x9E  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur1);
register_command("Change1", _("Change to \xF0\x9D\x85\x9E"), _("Change the current note to a \xF0\x9D\x85\x9E"), ChangeDur1);
register_command("Insert1", _("\xF0\x9D\x85\x9E"), _("Insert a \xF0\x9D\x85\x9E"), InsertDur1);
register_command("InsertRest1",  _("Insert a \xF0\x9D\x84\xBC") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\x9E"), InsertRest1);
register_command("Set1", _("Set Prevailing Duration to \xF0\x9D\x85\x9E"), _("Set the prevailing duration to \xF0\x9D\x85\x9E (subsequent notes entered will have this duration)"), SetDur1);

#undef NOTE1

#undef REST1

#undef NOTECHAR1

#undef RESTCHAR1
register_command("2", _("Insert/Append a \xF0\x9D\x85\x9F"), _("When appending, appends a \xF0\x9D\x85\x9F \nWith the cursor on a note inserts a \xF0\x9D\x85\x9F  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur2);
register_command("Change2", _("Change to \xF0\x9D\x85\x9F"), _("Change the current note to a \xF0\x9D\x85\x9F"), ChangeDur2);
register_command("Insert2", _("\xF0\x9D\x85\x9F"), _("Insert a \xF0\x9D\x85\x9F"), InsertDur2);
register_command("InsertRest2",  _("Insert a \xF0\x9D\x84\xBD") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\x9F"), InsertRest2);
register_command("Set2", _("Set Prevailing Duration to \xF0\x9D\x85\x9F"), _("Set the prevailing duration to \xF0\x9D\x85\x9F (subsequent notes entered will have this duration)"), SetDur2);

#undef NOTE2

#undef REST2

#undef NOTECHAR2

#undef RESTCHAR2
register_command("3", _("Insert/Append a \xF0\x9D\x85\xA0"), _("When appending, appends a \xF0\x9D\x85\xA0 \nWith the cursor on a note inserts a \xF0\x9D\x85\xA0  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur3);
register_command("Change3", _("Change to \xF0\x9D\x85\xA0"), _("Change the current note to a \xF0\x9D\x85\xA0"), ChangeDur3);
register_command("Insert3", _("\xF0\x9D\x85\xA0"), _("Insert a \xF0\x9D\x85\xA0"), InsertDur3);
register_command("InsertRest3",  _("Insert a \xF0\x9D\x84\xBE") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\xA0"), InsertRest3);
register_command("Set3", _("Set Prevailing Duration to \xF0\x9D\x85\xA0"), _("Set the prevailing duration to \xF0\x9D\x85\xA0 (subsequent notes entered will have this duration)"), SetDur3);

#undef NOTE3

#undef REST3

#undef NOTECHAR3

#undef RESTCHAR3
register_command("4", _("Insert/Append a \xF0\x9D\x85\xA1"), _("When appending, appends a \xF0\x9D\x85\xA1 \nWith the cursor on a note inserts a \xF0\x9D\x85\xA1  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur4);
register_command("Change4", _("Change to \xF0\x9D\x85\xA1"), _("Change the current note to a \xF0\x9D\x85\xA1"), ChangeDur4);
register_command("Insert4", _("\xF0\x9D\x85\xA1"), _("Insert a \xF0\x9D\x85\xA1"), InsertDur4);
register_command("InsertRest4",  _("Insert a \xF0\x9D\x84\xBF") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\xA1"), InsertRest4);
register_command("Set4", _("Set Prevailing Duration to \xF0\x9D\x85\xA1"), _("Set the prevailing duration to \xF0\x9D\x85\xA1 (subsequent notes entered will have this duration)"), SetDur4);

#undef NOTE4

#undef REST4

#undef NOTECHAR4

#undef RESTCHAR4
register_command("5", _("Insert/Append a \xF0\x9D\x85\xA2"), _("When appending, appends a \xF0\x9D\x85\xA2 \nWith the cursor on a note inserts a \xF0\x9D\x85\xA2  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur5);
register_command("Change5", _("Change to \xF0\x9D\x85\xA2"), _("Change the current note to a \xF0\x9D\x85\xA2"), ChangeDur5);
register_command("Insert5", _("\xF0\x9D\x85\xA2"), _("Insert a \xF0\x9D\x85\xA2"), InsertDur5);
register_command("InsertRest5",  _("Insert a \xF0\x9D\x85\x80") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\xA2"), InsertRest5);
register_command("Set5", _("Set Prevailing Duration to \xF0\x9D\x85\xA2"), _("Set the prevailing duration to \xF0\x9D\x85\xA2 (subsequent notes entered will have this duration)"), SetDur5);

#undef NOTE5

#undef REST5

#undef NOTECHAR5

#undef RESTCHAR5
register_command("6", _("Insert/Append a \xF0\x9D\x85\xA3"), _("When appending, appends a \xF0\x9D\x85\xA3 \nWith the cursor on a note inserts a \xF0\x9D\x85\xA3  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur6);
register_command("Change6", _("Change to \xF0\x9D\x85\xA3"), _("Change the current note to a \xF0\x9D\x85\xA3"), ChangeDur6);
register_command("Insert6", _("\xF0\x9D\x85\xA3"), _("Insert a \xF0\x9D\x85\xA3"), InsertDur6);
register_command("InsertRest6",  _("Insert a \xF0\x9D\x85\x81") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\xA3"), InsertRest6);
register_command("Set6", _("Set Prevailing Duration to \xF0\x9D\x85\xA3"), _("Set the prevailing duration to \xF0\x9D\x85\xA3 (subsequent notes entered will have this duration)"), SetDur6);

#undef NOTE6

#undef REST6

#undef NOTECHAR6

#undef RESTCHAR6
register_command("7", _("Insert/Append a \xF0\x9D\x85\xA4"), _("When appending, appends a \xF0\x9D\x85\xA4 \nWith the cursor on a note inserts a \xF0\x9D\x85\xA4  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur7);
register_command("Change7", _("Change to \xF0\x9D\x85\xA4"), _("Change the current note to a \xF0\x9D\x85\xA4"), ChangeDur7);
register_command("Insert7", _("\xF0\x9D\x85\xA4"), _("Insert a \xF0\x9D\x85\xA4"), InsertDur7);
register_command("InsertRest7",  _("Insert a \xF0\x9D\x85\x82") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\xA4"), InsertRest7);
register_command("Set7", _("Set Prevailing Duration to \xF0\x9D\x85\xA4"), _("Set the prevailing duration to \xF0\x9D\x85\xA4 (subsequent notes entered will have this duration)"), SetDur7);

#undef NOTE7

#undef REST7

#undef NOTECHAR7

#undef RESTCHAR7
register_command("8", _("Insert/Append a \xF0\x9D\x85\xA5"), _("When appending, appends a \xF0\x9D\x85\xA5 \nWith the cursor on a note inserts a \xF0\x9D\x85\xA5  before the current note\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\n - the MIDI keyboard will provide the pitch. Changes prevailing duration."), Dur8);
register_command("Change8", _("Change to \xF0\x9D\x85\xA5"), _("Change the current note to a \xF0\x9D\x85\xA5"), ChangeDur8);
register_command("Insert8", _("\xF0\x9D\x85\xA5"), _("Insert a \xF0\x9D\x85\xA5"), InsertDur8);
register_command("InsertRest8",  _("Insert a \xF0\x9D\x85\x83") ,  _("Inserts a rest at cursor position\nSets prevailing rhythm to \xF0\x9D\x85\xA5"), InsertRest8);
register_command("Set8", _("Set Prevailing Duration to \xF0\x9D\x85\xA5"), _("Set the prevailing duration to \xF0\x9D\x85\xA5 (subsequent notes entered will have this duration)"), SetDur8);

#undef NOTE8

#undef REST8

#undef NOTECHAR8

#undef RESTCHAR8
