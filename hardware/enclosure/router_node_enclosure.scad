// ============================================
// CULTIVIO AQUASENSE - ROUTER NODE ENCLOSURE
// ============================================
// OpenSCAD Parametric Design
// Export to STL for 3D printing
// 
// Material: PLA or PETG
// Layer Height: 0.2mm
// Infill: 15%
// 
// This is the SMALLEST enclosure - just a
// range extender, can be plugged into any
// USB outlet like a "smart plug"
// ============================================

// =====================
// PARAMETERS
// =====================

// PCB Dimensions (smallest PCB)
pcb_length = 30;       // mm - compact!
pcb_width = 25;        // mm
pcb_thickness = 1.6;   // mm

// Enclosure Settings
wall_thickness = 1.5;  // mm - thin for compact size
bottom_thickness = 1.5; // mm
top_thickness = 1.2;   // mm
pcb_clearance = 0.3;   // mm gap around PCB
pcb_standoff_height = 2; // mm height of PCB standoffs

// Component Heights (above PCB)
esp32_height = 3.5;    // ESP32-H2 module height
tallest_component = 5; // USB-C is tallest

// USB-C Port (direct plug design option)
usb_width = 9;         // mm
usb_height = 3.5;      // mm
usb_offset_x = 15;     // mm from left edge of PCB

// LED Window
led_diameter = 2.5;    // mm - smaller LEDs
led_spacing = 6;       // mm between LEDs
led_offset_x = 20;     // mm from left edge
led_offset_y = 12.5;   // mm from bottom edge

// Button
button_diameter = 5;   // mm
button_offset_x = 5;   // mm from left edge
button_offset_y = 12.5; // mm from bottom edge

// Mounting (optional - can also just sit on shelf)
mount_hole_diameter = 2.5; // M2.5 screw
keyhole_width = 5;     // For wall hanging

// Calculated Dimensions
inner_length = pcb_length + (pcb_clearance * 2);
inner_width = pcb_width + (pcb_clearance * 2);
inner_height = pcb_standoff_height + pcb_thickness + tallest_component + 1;

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
            cylinder(r=radius, h=height, $fn=24);
        translate([length-radius, radius, 0])
            cylinder(r=radius, h=height, $fn=24);
        translate([radius, width-radius, 0])
            cylinder(r=radius, h=height, $fn=24);
        translate([length-radius, width-radius, 0])
            cylinder(r=radius, h=height, $fn=24);
    }
}

// Simple PCB standoff
module pcb_standoff(height, diameter) {
    cylinder(d=diameter, h=height, $fn=16);
}

// Keyhole for wall mount
module keyhole(large_d, small_d, length) {
    hull() {
        cylinder(d=large_d, h=wall_thickness+0.2, $fn=24);
        translate([0, length, 0])
            cylinder(d=small_d, h=wall_thickness+0.2, $fn=24);
    }
}

// =====================
// BOTTOM CASE
// =====================

module bottom_case() {
    case_height = bottom_thickness + pcb_standoff_height + pcb_thickness + 3;
    
    difference() {
        // Outer shell
        rounded_box(outer_length, outer_width, case_height, 2);
        
        // Inner cavity
        translate([wall_thickness, wall_thickness, bottom_thickness])
            rounded_box(inner_length, inner_width, case_height, 1.5);
        
        // USB-C cutout
        translate([wall_thickness + pcb_clearance + usb_offset_x - usb_width/2, 
                   -0.1, 
                   bottom_thickness + pcb_standoff_height])
            cube([usb_width, wall_thickness + 0.2, usb_height + 1.5]);
        
        // Keyhole wall mount (back)
        translate([outer_length/2, outer_width - wall_thickness/2, -0.1])
            keyhole(5, 3, 6);
    }
    
    // PCB Standoffs (4 corners)
    standoff_positions = [
        [wall_thickness + pcb_clearance + 2.5, 
         wall_thickness + pcb_clearance + 2.5],
        [wall_thickness + pcb_clearance + pcb_length - 2.5, 
         wall_thickness + pcb_clearance + 2.5],
        [wall_thickness + pcb_clearance + 2.5, 
         wall_thickness + pcb_clearance + pcb_width - 2.5],
        [wall_thickness + pcb_clearance + pcb_length - 2.5, 
         wall_thickness + pcb_clearance + pcb_width - 2.5]
    ];
    
    for (pos = standoff_positions) {
        translate([pos[0], pos[1], bottom_thickness])
            pcb_standoff(pcb_standoff_height, 4);
    }
}

// =====================
// TOP CASE (LID)
// =====================

module top_case() {
    lid_height = inner_height - pcb_standoff_height - pcb_thickness - 3 + top_thickness;
    
    difference() {
        // Outer shell
        rounded_box(outer_length, outer_width, lid_height, 2);
        
        // Inner lip for snap fit
        translate([wall_thickness - 0.15, wall_thickness - 0.15, top_thickness])
            rounded_box(inner_length + 0.3, inner_width + 0.3, lid_height, 1.5);
        
        // LED windows
        translate([wall_thickness + pcb_clearance + led_offset_x, 
                   wall_thickness + pcb_clearance + led_offset_y, -0.1])
            cylinder(d=led_diameter, h=top_thickness + 0.2, $fn=24);
        translate([wall_thickness + pcb_clearance + led_offset_x + led_spacing, 
                   wall_thickness + pcb_clearance + led_offset_y, -0.1])
            cylinder(d=led_diameter, h=top_thickness + 0.2, $fn=24);
        
        // Button access hole
        translate([wall_thickness + pcb_clearance + button_offset_x, 
                   wall_thickness + pcb_clearance + button_offset_y, -0.1])
            cylinder(d=button_diameter, h=top_thickness + 0.2, $fn=24);
    }
    
    // Logo/Text (embossed) - smaller for compact case
    translate([outer_length/2, outer_width/2 + 3, 0])
        linear_extrude(height=0.4)
            text("CULTIVIO", size=3.5, halign="center", valign="center", font="Arial:style=Bold");
    
    translate([outer_length/2, outer_width/2 - 4, 0])
        linear_extrude(height=0.4)
            text("Router", size=2.5, halign="center", valign="center", font="Arial");
    
    // Signal strength icon
    translate([outer_length - 6, outer_width/2, 0]) {
        // WiFi-like icon (signal bars)
        for (i = [0:2]) {
            translate([i*2, 0, 0])
                linear_extrude(height=0.4)
                    square([1.2, 2 + i*1.5], center=true);
        }
    }
}

// =====================
// WALL PLUG HOUSING (ALTERNATIVE)
// =====================
// This version integrates with a USB charger
// to create a plug-in range extender

module wall_plug_housing() {
    // Housing that wraps around a USB charger cube
    charger_width = 30;
    charger_depth = 30;
    charger_height = 25;
    
    housing_wall = 1.5;
    
    difference() {
        // Outer housing
        translate([0, 0, 0])
            rounded_box(charger_width + housing_wall*2 + 5, 
                       charger_depth + housing_wall*2,
                       charger_height + outer_height + housing_wall, 3);
        
        // Charger cavity
        translate([housing_wall, housing_wall, housing_wall])
            cube([charger_width, charger_depth, charger_height]);
        
        // PCB cavity (top)
        translate([housing_wall + 2.5, housing_wall + 2.5, 
                   housing_wall + charger_height])
            rounded_box(inner_length, inner_width, outer_height, 1.5);
        
        // USB cable channel
        translate([charger_width/2 + housing_wall - 3, 
                   charger_depth + housing_wall - 0.1, 
                   housing_wall + charger_height/2])
            cube([6, housing_wall + 0.2, 8]);
        
        // LED windows
        translate([housing_wall + 2.5 + pcb_clearance + led_offset_x, 
                   housing_wall + 2.5 + pcb_clearance + led_offset_y, 
                   housing_wall + charger_height + outer_height - 0.1])
            cylinder(d=led_diameter, h=housing_wall + 0.2, $fn=24);
        translate([housing_wall + 2.5 + pcb_clearance + led_offset_x + led_spacing, 
                   housing_wall + 2.5 + pcb_clearance + led_offset_y, 
                   housing_wall + charger_height + outer_height - 0.1])
            cylinder(d=led_diameter, h=housing_wall + 0.2, $fn=24);
        
        // Prong access
        translate([-0.1, charger_depth/2 - 10 + housing_wall, housing_wall])
            cube([housing_wall + 0.2, 20, 15]);
    }
}

// =====================
// RENDER
// =====================

// Standard enclosure
color("LightSteelBlue")
    bottom_case();

color("SteelBlue")
    translate([outer_length + 10, 0, 0])
        top_case();

// Wall plug housing (alternative)
color("DarkSlateGray")
    translate([0, outer_width + 20, 0])
        wall_plug_housing();

// For export, uncomment one of the following:
// bottom_case();
// top_case();
// wall_plug_housing();

// =====================
// EXPORT INSTRUCTIONS
// =====================
// 1. Open this file in OpenSCAD
// 2. Uncomment desired part at the end
// 3. Render (F6)
// 4. Export as STL (F7)
//
// Print Settings:
// - Material: PLA or PETG
// - Layer: 0.2mm
// - Infill: 15%
// - No supports needed
// - Print time: ~45 min per part

