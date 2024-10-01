package handlers

import "github.com/gofiber/fiber/v2"

func Index(c *fiber.Ctx) error {
	return c.Render("index", fiber.Map{}, "layouts/main")
}

func Comments(c *fiber.Ctx) error {
	return c.Render("comments", fiber.Map{}, "layouts/main")
}

func Error404(c *fiber.Ctx) error {
	return c.Render("404", fiber.Map{})
}