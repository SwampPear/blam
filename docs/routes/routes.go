package routes

import (
    "github.com/gofiber/fiber/v2"
    "github.com/SwampPear/blam/docs/handlers"
)

func SetupRoutes(app *fiber.App) {
	app.Get("/", handlers.Index)
	app.Get("/comments", handlers.Index)
	app.Use(handlers.Error404)
}