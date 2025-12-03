// ============================================
// CULTIVIO AQUASENSE - SENSOR NODE ENCLOSURE
// ============================================
// OpenSCAD Parametric Design
// Export to STL for 3D printing
// 
// Material: PETG (water resistant)
// Layer Height: 0.2mm
// Infill: 20%
// ============================================

// =====================
// PARAMETERS
// =====================

// PCB Dimensions
pcb_length = 45;       // mm
pcb_width = 35;        // mm
pcb_thickness = 1.6;   // mm

// Enclosure Settings
wall_thickness = 2.0;  // mm
bottom_thickness = 2.0; // mm
top_thickness = 1.5;   // mm
pcb_clearance = 0.5;   // mm gap around PCB
pcb_standoff_height = 3; // mm height of PCB standoffs

// Component Heights (above PCB)
esp32_height = 3.5;    // ESP32-H2 module height
tallest_component = 8; // Tallest component (USB-C, etc.)
cable_gland_diameter = 8; // mm for sensor cable

// USB-C Port
usb_width = 9;         // mm
usb_height = 3.5;      // mm
usb_offset_x = 22.5;   // mm from left edge of PCB

// LED Window
led_diameter = 3;      // mm
led_spacing = 8;       // mm between LEDs
led_offset_x = 35;     // mm from left edge
led_offset_y = 17.5;   // mm from bottom edge

// Button
button_diameter = 6;   // mm
button_offset_x = 5;   // mm from left edge
button_offset_y = 17.5; // mm from bottom edge

// Mounting Holes
mount_hole_diameter = 3.2; // M3 screw
mount_hole_offset = 3;     // mm from corner

// Snap-fit Settings
snap_height = 2;       // mm
snap_depth = 1;        // mm
snap_tolerance = 0.2;  // mm

// Calculated Dimensions
inner_length = pcb_length + (pcb_clearance * 2);
inner_width = pcb_width + (pcb_clearance * 2);
inner_height = pcb_standoff_height + pcb_thickness + tallest_component + 2;

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

// PCB Standoff
module pcb_standoff(height, outer_d, inner_d) {
    difference() {
        cylinder(d=outer_d, h=height, $fn=32);
        translate([0, 0, -0.1])
            cylinder(d=inner_d, h=height+0.2, $fn=32);
    }
}

// Snap fit clip
module snap_clip(length, height, depth) {
    translate([0, 0, 0])
    linear_extrude(height=length)
    polygon(points=[
        [0, 0],
        [depth, 0],
        [depth, height-depth],
        [0, height]
    ]);
}

// =====================
// BOTTOM CASE
// =====================

module bottom_case() {
    difference() {
        // Outer shell
        rounded_box(outer_length, outer_width, 
                   bottom_thickness + pcb_standoff_height + pcb_thickness + 5, 
                   3);
        
        // Inner cavity
        translate([wall_thickness, wall_thickness, bottom_thickness])
            rounded_box(inner_length, inner_width, 
                       pcb_standoff_height + pcb_thickness + 5 + 0.1, 
                       2);
        
        // USB-C cutout
        translate([wall_thickness + pcb_clearance + usb_offset_x - usb_width/2, 
                   -0.1, 
                   bottom_thickness + pcb_standoff_height])
            cube([usb_width, wall_thickness + 0.2, usb_height + 2]);
        
        // Cable gland hole (for sensor cable)
        translate([outer_length/2, outer_width, outer_height/2])
            rotate([90, 0, 0])
                cylinder(d=cable_gland_diameter, h=wall_thickness + 0.2, $fn=32);
        
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
    }
    
    // PCB Standoffs
    standoff_positions = [
        [wall_thickness + pcb_clearance + 3, 
         wall_thickness + pcb_clearance + 3],
        [wall_thickness + pcb_clearance + pcb_length - 3, 
         wall_thickness + pcb_clearance + 3],
        [wall_thickness + pcb_clearance + 3, 
         wall_thickness + pcb_clearance + pcb_width - 3],
        [wall_thickness + pcb_clearance + pcb_length - 3, 
         wall_thickness + pcb_clearance + pcb_width - 3]
    ];
    
    for (pos = standoff_positions) {
        translate([pos[0], pos[1], bottom_thickness])
            pcb_standoff(pcb_standoff_height, 5, 2);
    }
}

// =====================
// TOP CASE (LID)
// =====================

module top_case() {
    lid_height = inner_height - pcb_standoff_height - pcb_thickness - 5 + top_thickness;
    
    difference() {
        // Outer shell
        rounded_box(outer_length, outer_width, lid_height, 3);
        
        // Inner lip for snap fit
        translate([wall_thickness - snap_tolerance, 
                   wall_thickness - snap_tolerance, 
                   top_thickness])
            rounded_box(inner_length + snap_tolerance*2, 
                       inner_width + snap_tolerance*2, 
                       lid_height, 2);
        
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
        
        // Ventilation slots (optional)
        for (i = [0:3]) {
            translate([10 + i*10, wall_thickness/2, -0.1])
                cube([5, outer_width - wall_thickness, top_thickness + 0.2]);
        }
    }
    
    // Logo/Text (embossed)
    translate([outer_length/2, outer_width/2, 0])
        linear_extrude(height=0.5)
            text("CULTIVIO", size=5, halign="center", valign="center", font="Arial:style=Bold");
}

// =====================
// RENDER
// =====================

// Render bottom case
color("DarkSlateGray")
    bottom_case();

// Render top case (offset for visualization)
color("SlateGray")
    translate([outer_length + 10, 0, 0])
        top_case();

// For export, uncomment one of the following:
// bottom_case();
// top_case();

// =====================
// EXPORT INSTRUCTIONS
// =====================
// 1. Open this file in OpenSCAD
// 2. Uncomment "bottom_case();" at the end
// 3. Render (F6)
// 4. Export as STL (F7)
// 5. Repeat for "top_case();"

