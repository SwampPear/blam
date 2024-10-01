package handlers

import "github.com/gofiber/fiber/v2"

func Index(c *fiber.Ctx) error {
	return c.Render("index", fiber.Map{}, "layouts/main")
}

func Bloop(c *fiber.Ctx) error {
	return c.Render("bloop", fiber.Map{}, "layouts/main")
}

func Comments(c *fiber.Ctx) error {
	return c.Render("comments", fiber.Map{}, "layouts/main")
}

func DataTypes(c *fiber.Ctx) error {
	return c.Render("data_types", fiber.Map{}, "layouts/main")
}

func Functions(c *fiber.Ctx) error {
	return c.Render("functions", fiber.Map{}, "layouts/main")
}

func Variables(c *fiber.Ctx) error {
	return c.Render("variables", fiber.Map{}, "layouts/main")
}


func ControlFlow(c *fiber.Ctx) error {
	return c.Render("control_flow", fiber.Map{}, "layouts/main")
}

func Error404(c *fiber.Ctx) error {
	return c.Render("404", fiber.Map{})
}