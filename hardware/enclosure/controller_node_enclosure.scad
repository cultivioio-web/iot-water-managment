// ============================================
// CULTIVIO AQUASENSE - CONTROLLER NODE ENCLOSURE
// ============================================
// OpenSCAD Parametric Design
// Export to STL for 3D printing
// 
// Material: ABS (heat resistant)
// Layer Height: 0.2mm
// Infill: 25%
// ============================================

// =====================
// PARAMETERS
// =====================

// PCB Dimensions
pcb_length = 45;       // mm
pcb_width = 35;        // mm
pcb_thickness = 1.6;   // mm

// Enclosure Settings
wall_thickness = 2.5;  // mm (thicker for relay heat)
bottom_thickness = 2.5; // mm
top_thickness = 2.0;   // mm
pcb_clearance = 0.5;   // mm gap around PCB
pcb_standoff_height = 4; // mm height of PCB standoffs

// Component Heights (above PCB)
esp32_height = 3.5;    // ESP32-H2 module height
relay_height = 16;     // Relay height (tallest)
screw_terminal_height = 10; // Screw terminal height

// USB-C Port
usb_width = 9;         // mm
usb_height = 3.5;      // mm
usb_offset_x = 27.5;   // mm from left edge of PCB

// Screw Terminal Access
terminal_width = 10;   // mm
terminal_height = 12;  // mm
terminal1_offset_x = 40; // First terminal position
terminal2_offset_x = 50; // Second terminal position

// LED Window
led_diameter = 3;      // mm
led_spacing = 8;       // mm between LEDs
led_offset_x = 10;     // mm from left edge
led_offset_y = 22.5;   // mm from bottom edge

// Button
button_diameter = 6;   // mm
button_offset_x = 5;   // mm from left edge
button_offset_y = 10;  // mm from bottom edge

// Buzzer Holes
buzzer_offset_x = 25;  // mm from left edge
buzzer_offset_y = 35;  // mm from bottom edge
buzzer_holes = 7;      // Number of sound holes

// Ventilation (for relay heat)
vent_slot_width = 3;   // mm
vent_slot_length = 15; // mm
vent_spacing = 6;      // mm

// Mounting Holes
mount_hole_diameter = 3.2; // M3 screw
mount_hole_offset = 4;     // mm from corner

// DIN Rail Mount (optional)
din_rail_width = 35;   // Standard DIN rail
din_rail_slot_width = 7;
din_rail_slot_depth = 5;

// Calculated Dimensions
inner_length = pcb_length + (pcb_clearance * 2);
inner_width = pcb_width + (pcb_clearance * 2);
inner_height = pcb_standoff_height + pcb_thickness + relay_height + 3;

outer_length = inner_length + (wall_thickness * 2);
outer_width = inner_width + (wall_thickness * 2);
outer_height = inner_height + bottom_thickness + top_thickness;

// =====================
// MODULES
// =====================

// Rounded box module
module rounded_box(length, width, height, radius) {
    hull() {
        translate([radius, radius, 0])
            cylinder(r=radius, h=height, $fn=32);
        translate([length-radius, radius, 0])
            cylinder(r=radius, h=height, $fn=32);
        translate([radius, width-radius, 0])
            cylinder(r=radius, h=height, $fn=32);
        translate([length-radius, width-radius, 0])
            cylinder(r=radius, h=height, $fn=32);
    }
}

// PCB Standoff with screw hole
module pcb_standoff(height, outer_d, inner_d) {
    difference() {
        cylinder(d=outer_d, h=height, $fn=32);
        translate([0, 0, height - 3])
            cylinder(d=inner_d, h=3.1, $fn=32);
    }
}

// Ventilation grid
module vent_grid(width, length, slot_w, spacing) {
    num_slots = floor(width / spacing);
    for (i = [0:num_slots-1]) {
        translate([i * spacing, 0, 0])
            cube([slot_w, length, wall_thickness + 0.2]);
    }
}

// Buzzer sound holes pattern
module buzzer_holes(diameter, count, spacing) {
    for (angle = [0:360/count:360-1]) {
        rotate([0, 0, angle])
            translate([spacing, 0, 0])
                cylinder(d=diameter, h=top_thickness + 0.2, $fn=16);
    }
    // Center hole
    cylinder(d=diameter, h=top_thickness + 0.2, $fn=16);
}

// =====================
// BOTTOM CASE
// =====================

module bottom_case() {
    case_height = bottom_thickness + pcb_standoff_height + pcb_thickness + 8;
    
    difference() {
        // Outer shell
        rounded_box(outer_length, outer_width, case_height, 3);
        
        // Inner cavity
        translate([wall_thickness, wall_thickness, bottom_thickness])
            rounded_box(inner_length, inner_width, case_height, 2);
        
        // USB-C cutout
        translate([wall_thickness + pcb_clearance + usb_offset_x - usb_width/2, 
                   -0.1, 
                   bottom_thickness + pcb_standoff_height])
            cube([usb_width, wall_thickness + 0.2, usb_height + 2]);
        
        // Screw Terminal cutouts (back side)
        translate([wall_thickness + pcb_clearance + terminal1_offset_x - terminal_width/2, 
                   outer_width - wall_thickness - 0.1, 
                   bottom_thickness + pcb_standoff_height])
            cube([terminal_width, wall_thickness + 0.2, terminal_height]);
        translate([wall_thickness + pcb_clearance + terminal2_offset_x - terminal_width/2, 
                   outer_width - wall_thickness - 0.1, 
                   bottom_thickness + pcb_standoff_height])
            cube([terminal_width, wall_thickness + 0.2, terminal_height]);
        
        // Side ventilation slots
        translate([wall_thickness + 5, -0.1, case_height - 10])
            vent_grid(outer_length - wall_thickness*2 - 10, wall_thickness + 0.2, 
                     vent_slot_width, vent_spacing);
        translate([wall_thickness + 5, outer_width - wall_thickness - 0.1, case_height - 10])
            vent_grid(outer_length - wall_thickness*2 - 10, wall_thickness + 0.2, 
                     vent_slot_width, vent_spacing);
        
        // Mounting holes
        translate([mount_hole_offset + wall_thickness, 
                   mount_hole_offset + wall_thickness, -0.1])
            cylinder(d=mount_hole_diameter, h=bottom_thickness + 0.2, $fn=32);
        translate([outer_length - mount_hole_offset - wall_thickness, 
                   mount_hole_offset + wall_thickness, -0.1])
            cylinder(d=mount_hole_diameter, h=bottom_thickness + 0.2, $fn=32);
        translate([mount_hole_offset + wall_thickness, 
                   outer_width - mount_hole_offset - wall_thickness, -0.1])
            cylinder(d=mount_hole_diameter, h=bottom_thickness + 0.2, $fn=32);
        translate([outer_length - mount_hole_offset - wall_thickness, 
                   outer_width - mount_hole_offset - wall_thickness, -0.1])
            cylinder(d=mount_hole_diameter, h=bottom_thickness + 0.2, $fn=32);
        
        // DIN Rail mounting slot (bottom)
        translate([(outer_length - din_rail_width)/2, 
                   outer_width/2 - din_rail_slot_width/2, -0.1])
            cube([din_rail_width, din_rail_slot_width, din_rail_slot_depth]);
    }
    
    // PCB Standoffs
    standoff_positions = [
        [wall_thickness + pcb_clearance + 4, 
         wall_thickness + pcb_clearance + 4],
        [wall_thickness + pcb_clearance + pcb_length - 4, 
         wall_thickness + pcb_clearance + 4],
        [wall_thickness + pcb_clearance + 4, 
         wall_thickness + pcb_clearance + pcb_width - 4],
        [wall_thickness + pcb_clearance + pcb_length - 4, 
         wall_thickness + pcb_clearance + pcb_width - 4]
    ];
    
    for (pos = standoff_positions) {
        translate([pos[0], pos[1], bottom_thickness])
            pcb_standoff(pcb_standoff_height, 6, 2.5);
    }
    
    // Internal ribs for strength
    translate([wall_thickness, outer_width/2 - 1, bottom_thickness])
        cube([inner_length, 2, pcb_standoff_height - 1]);
}

// =====================
// TOP CASE (LID)
// =====================

module top_case() {
    lid_height = inner_height - pcb_standoff_height - pcb_thickness - 8 + top_thickness;
    
    difference() {
        // Outer shell
        rounded_box(outer_length, outer_width, lid_height, 3);
        
        // Inner lip for snap fit
        translate([wall_thickness - 0.2, wall_thickness - 0.2, top_thickness])
            rounded_box(inner_length + 0.4, inner_width + 0.4, lid_height, 2);
        
        // LED windows
        translate([wall_thickness + pcb_clearance + led_offset_x, 
                   wall_thickness + pcb_clearance + led_offset_y, -0.1])
            cylinder(d=led_diameter, h=top_thickness + 0.2, $fn=32);
        translate([wall_thickness + pcb_clearance + led_offset_x + led_spacing, 
                   wall_thickness + pcb_clearance + led_offset_y, -0.1])
            cylinder(d=led_diameter, h=top_thickness + 0.2, $fn=32);
        
        // Button access hole
        translate([wall_thickness + pcb_clearance + button_offset_x, 
                   wall_thickness + pcb_clearance + button_offset_y, -0.1])
            cylinder(d=button_diameter, h=top_thickness + 0.2, $fn=32);
        
        // Buzzer sound holes
        translate([wall_thickness + pcb_clearance + buzzer_offset_x, 
                   wall_thickness + pcb_clearance + buzzer_offset_y, -0.1])
            buzzer_holes(2, buzzer_holes, 4);
        
        // Top ventilation grid
        translate([outer_length - 25, wall_thickness + 5, -0.1])
            vent_grid(20, outer_width - wall_thickness*2 - 10, 
                     vent_slot_width, vent_spacing);
    }
    
    // Logo/Text (embossed)
    translate([outer_length/2, outer_width/2 + 10, 0])
        linear_extrude(height=0.5)
            text("CULTIVIO", size=6, halign="center", valign="center", font="Arial:style=Bold");
    
    translate([outer_length/2, outer_width/2 - 5, 0])
        linear_extrude(height=0.5)
            text("AquaSense", size=4, halign="center", valign="center", font="Arial");
    
    // Warning label area
    translate([outer_length/2, 10, 0])
        linear_extrude(height=0.3)
            text("⚡ 230V", size=3, halign="center", valign="center");
}

// =====================
// WALL MOUNT BRACKET (OPTIONAL)
// =====================

module wall_mount_bracket() {
    bracket_width = 20;
    bracket_height = 40;
    bracket_depth = 25;
    
    difference() {
        union() {
            // Vertical plate
            cube([bracket_width, 3, bracket_height]);
            // Horizontal shelf
            translate([0, 0, 0])
                cube([bracket_width, bracket_depth, 3]);
            // Side supports
            translate([0, 0, 0])
                rotate([0, 90, 0])
                    linear_extrude(height=3)
                        polygon(points=[[0,0], [0,bracket_depth], [-bracket_height,0]]);
            translate([bracket_width-3, 0, 0])
                rotate([0, 90, 0])
                    linear_extrude(height=3)
                        polygon(points=[[0,0], [0,bracket_depth], [-bracket_height,0]]);
        }
        
        // Wall mount holes
        translate([bracket_width/2, -0.1, bracket_height - 10])
            rotate([-90, 0, 0])
                cylinder(d=4, h=3.2, $fn=32);
        translate([bracket_width/2, -0.1, 10])
            rotate([-90, 0, 0])
                cylinder(d=4, h=3.2, $fn=32);
    }
}

// =====================
// RENDER
// =====================

// Render bottom case
color("DarkOliveGreen")
    bottom_case();

// Render top case (offset for visualization)
color("OliveDrab")
    translate([outer_length + 10, 0, 0])
        top_case();

// Render wall mount bracket (offset)
color("DimGray")
    translate([0, outer_width + 20, 0])
        wall_mount_bracket();

// For export, uncomment one of the following:
// bottom_case();
// top_case();
// wall_mount_bracket();

// =====================
// EXPORT INSTRUCTIONS
// =====================
// 1. Open this file in OpenSCAD
// 2. Uncomment desired part at the end
// 3. Render (F6)
// 4. Export as STL (F7)
// 5. Repeat for each part
//
// Print Settings:
// - Material: ABS or PETG
// - Layer: 0.2mm
// - Infill: 25%
// - Supports: Yes (for overhangs)
// - Brim: Recommended for ABS

